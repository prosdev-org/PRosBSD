#include <cmos.h>
#include <ctype.h>
#include <extrns.h>
#include <fs/fat32.h>
#include <pata_pio.h>
#include <string.h>

bool read_sectors(fat32_ctx_t *ctx, uint32_t lba, uint32_t count, void *buffer) {
    for (uint32_t i = 0; i < count; i++) {
        if (!pata_pio_read_sector(lba + i, (uint8_t *) buffer + i * 512)) {
            return false;
        }
    }
    return true;
}

bool write_sectors(fat32_ctx_t *ctx, uint32_t lba, uint32_t count, void *buffer) {
    for (uint32_t i = 0; i < count; i++) {
        if (!pata_pio_write_sector(lba + i, (uint8_t *) buffer + i * 512)) {
            return false;
        }
    }
    return true;
}

uint32_t cluster_to_lba(fat32_ctx_t *ctx, uint32_t cluster) {
    return ctx->data_start + (cluster - 2) * ctx->sectors_per_cluster;
}

uint16_t time_to_fat(uint8_t hours, uint8_t minutes, uint8_t seconds) {
    return ((hours & 0x1F) << 11) | ((minutes & 0x3F) << 5) | ((seconds / 2) & 0x1F);
}

uint16_t date_to_fat(uint8_t day, uint8_t month, uint8_t year) {
    uint8_t fat_year = (year >= 80) ? (year - 80) : (year + 20);
    return ((fat_year & 0x7F) << 9) | ((month & 0x0F) << 5) | (day & 0x1F);
}


bool fat32_init(fat32_ctx_t *ctx, uint32_t lba_start) {
    memset(ctx, 0, sizeof(fat32_ctx_t));
    ctx->lba_start = lba_start;

    uint8_t boot_sector[512];
    if (!pata_pio_read_sector(lba_start, boot_sector)) {
        return false;
    }

    fat32_boot_sector_t bs;
    memcpy(&bs, boot_sector, sizeof(bs));

    if (bs.signature != 0x28 && bs.signature != 0x29) {
        return false;
    }

    ctx->bytes_per_sector = bs.bytes_per_sector;
    ctx->sectors_per_cluster = bs.sectors_per_cluster;
    ctx->cluster_size = ctx->bytes_per_sector * ctx->sectors_per_cluster;
    ctx->fat_count = bs.fat_count;

    uint32_t reserved_sectors = bs.reserved_sectors;
    uint32_t fat_size = (bs.fat_size_16 != 0) ? bs.fat_size_16 : bs.fat_size_32;

    ctx->fat_start = lba_start + reserved_sectors;
    ctx->fat_size = fat_size;
    ctx->data_start = ctx->fat_start + (ctx->fat_count * fat_size);
    ctx->root_cluster = bs.root_cluster;
    ctx->total_clusters =
            (bs.total_sectors_32 - reserved_sectors - (ctx->fat_count * fat_size)) / bs.sectors_per_cluster;

    if (bs.fs_info_sector != 0) {
        pata_pio_read_sector(lba_start + bs.fs_info_sector, (uint8_t *) &ctx->fsinfo);
    }

    return true;
}

bool fat32_update_timestamps(fat32_ctx_t *ctx, uint32_t dir_cluster, const char *name, bool update_creation,
                             bool update_access, bool update_modification) {
    uint8_t seconds, minutes, hours, day, month, year;
    get_rtc_time(&seconds, &minutes, &hours);
    get_rtc_date(&day, &month, &year);

    uint16_t fat_time = time_to_fat(hours, minutes, seconds);
    uint16_t fat_date = date_to_fat(day, month, year);
    uint8_t fat_time_tenths = 0;

    fat32_dir_entry_t entry;
    if (!fat32_find_file(ctx, dir_cluster, name, &entry)) {
        return false;
    }

    uint8_t *cluster_data = malloc(ctx->cluster_size);
    if (!cluster_data)
        return false;

    uint32_t lba = cluster_to_lba(ctx, dir_cluster);
    if (!read_sectors(ctx, lba, ctx->sectors_per_cluster, cluster_data)) {
        free(cluster_data);
        return false;
    }

    fat32_dir_entry_t *de = (fat32_dir_entry_t *) cluster_data;
    for (uint32_t i = 0; i < ctx->cluster_size / sizeof(fat32_dir_entry_t); i++) {
        if (memcmp(de[i].name, entry.name, 11) == 0) {
            if (update_creation) {
                de[i].creation_time_tenths = fat_time_tenths;
                de[i].creation_time = fat_time;
                de[i].creation_date = fat_date;
            }
            if (update_access) {
                de[i].last_access_date = fat_date;
            }
            if (update_modification) {
                de[i].last_write_time = fat_time;
                de[i].last_write_date = fat_date;
            }

            bool success = write_sectors(ctx, lba, ctx->sectors_per_cluster, cluster_data);
            free(cluster_data);
            return success;
        }
    }

    free(cluster_data);
    return false;
}

bool fat32_open_root(fat32_ctx_t *ctx, fat32_file_t *file) {
    memset(file, 0, sizeof(fat32_file_t));
    file->ctx = ctx;
    file->start_cluster = ctx->root_cluster;
    file->current_cluster = ctx->root_cluster;
    file->is_dir = true;
    return true;
}

void split_path(const char *path, char *parent, char *name) {
    const char *last_slash = strrchr(path, '/');
    if (!last_slash) {
        strcpy(parent, "");
        strcpy(name, path);
        return;
    }

    size_t parent_len = last_slash - path;
    if (parent_len == 0) {
        strcpy(parent, "/");
    } else {
        strncpy(parent, path, parent_len);
        parent[parent_len] = '\0';
    }
    strcpy(name, last_slash + 1);
}

uint32_t find_free_cluster(fat32_ctx_t *ctx) {
    uint8_t sector[512];
    for (uint32_t sector_num = 0; sector_num < ctx->fat_size; sector_num++) {
        if (!read_sectors(ctx, ctx->fat_start + sector_num, 1, sector))
            return 0;

        for (uint32_t i = 0; i < ctx->bytes_per_sector; i += 4) {
            uint32_t entry = *(uint32_t *) (sector + i) & FAT32_MASK;
            if (entry == 0) {
                uint32_t cluster = (sector_num * ctx->bytes_per_sector + i) / 4;
                if (cluster >= 2 && cluster < ctx->total_clusters + 2)
                    return cluster;
            }
        }
    }
    return 0;
}

bool set_fat_entry(fat32_ctx_t *ctx, uint32_t cluster, uint32_t value) {
    value &= FAT32_MASK;
    uint32_t fat_offset = cluster * 4;
    uint32_t fat_sector = ctx->fat_start + (fat_offset / ctx->bytes_per_sector);
    uint32_t entry_offset = fat_offset % ctx->bytes_per_sector;

    uint8_t sector[512];
    if (!read_sectors(ctx, fat_sector, 1, sector))
        return false;

    *(uint32_t *) (sector + entry_offset) = value;

    if (!write_sectors(ctx, fat_sector, 1, sector))
        return false;

    // update all FAT copies
    for (int i = 1; i < ctx->fat_count; i++) {
        if (!write_sectors(ctx, fat_sector + i * ctx->fat_size, 1, sector))
            return false;
    }

    return true;
}

bool create_direntry(fat32_ctx_t *ctx, uint32_t dir_cluster, const char *name, uint8_t attrs, uint32_t first_cluster,
                     uint32_t size) {
    uint8_t *cluster_data = malloc(ctx->cluster_size);
    if (!cluster_data)
        return false;

    uint32_t lba = cluster_to_lba(ctx, dir_cluster);
    if (!read_sectors(ctx, lba, ctx->sectors_per_cluster, cluster_data)) {
        free(cluster_data);
        return false;
    }

    fat32_dir_entry_t *de = NULL;
    for (uint32_t i = 0; i < ctx->cluster_size; i += sizeof(fat32_dir_entry_t)) {
        de = (fat32_dir_entry_t *) (cluster_data + i);
        if (de->name[0] == 0x00 || de->name[0] == 0xE5) {
            memset(de, 0, sizeof(fat32_dir_entry_t));

            // 8.3
            const char *dot = strchr(name, '.');
            size_t name_len = dot ? (dot - name) : strlen(name);
            name_len = (name_len > 8) ? 8 : name_len;
            for (size_t j = 0; j < name_len; j++) {
                de->name[j] = toupper(name[j]);
            }

            for (size_t j = name_len; j < 8; j++) {
                de->name[j] = ' ';
            }

            if (dot) {
                const char *ext = dot + 1;
                size_t ext_len = strlen(ext);
                ext_len = (ext_len > 3) ? 3 : ext_len;
                for (size_t j = 0; j < ext_len; j++) {
                    de->name[8 + j] = toupper(ext[j]);
                }
                for (size_t j = ext_len; j < 3; j++) {
                    de->name[8 + j] = ' ';
                }
            } else {
                for (size_t j = 0; j < 3; j++) {
                    de->name[8 + j] = ' ';
                }
            }

            de->attributes = attrs;
            de->first_cluster_high = (first_cluster >> 16) & 0xFFFF;
            de->first_cluster_low = first_cluster & 0xFFFF;
            de->file_size = size;

            uint8_t seconds, minutes, hours, day, month, year;
            get_rtc_time(&seconds, &minutes, &hours);
            get_rtc_date(&day, &month, &year);

            uint16_t fat_time = time_to_fat(hours, minutes, seconds);
            uint16_t fat_date = date_to_fat(day, month, year);

            de->creation_time_tenths = 0;
            de->creation_time = fat_time;
            de->creation_date = fat_date;
            de->last_access_date = fat_date;
            de->last_write_time = fat_time;
            de->last_write_date = fat_date;

            // write back
            bool success = write_sectors(ctx, lba, ctx->sectors_per_cluster, cluster_data);
            free(cluster_data);
            return success;
        }
    }

    free(cluster_data);
    return false;
}

bool fat32_find_file(fat32_ctx_t *ctx, uint32_t start_cluster, const char *name, fat32_dir_entry_t *entry) {
    fat32_file_t dir;
    memset(&dir, 0, sizeof(dir));
    dir.ctx = ctx;
    dir.start_cluster = start_cluster;
    dir.current_cluster = start_cluster;
    dir.is_dir = true;

    fat32_dir_entry_t de;
    while (fat32_read_dir(&dir, &de)) {
        if (de.name[0] == 0xE5)
            continue;
        if (de.attributes == ATTR_VOLUME_ID || de.attributes == ATTR_LONG_NAME)
            continue;

        char entry_name[13];
        int i = 0;
        for (int j = 0; j < 8; j++) {
            if (de.name[j] == ' ')
                break;
            entry_name[i++] = de.name[j];
        }

        if (de.name[8] != ' ') {
            entry_name[i++] = '.';
            for (int j = 8; j < 11; j++) {
                if (de.name[j] == ' ')
                    break;
                entry_name[i++] = de.name[j];
            }
        }
        entry_name[i] = '\0';

        if (strcasecmp(entry_name, name) == 0) {
            if (entry)
                *entry = de;
            return true;
        }
    }
    return false;
}

bool fat32_open_file(fat32_ctx_t *ctx, fat32_file_t *file, const char *path) {
    memset(file, 0, sizeof(fat32_file_t));
    file->ctx = ctx;

    if (strcmp(path, "/") == 0) {
        file->start_cluster = ctx->root_cluster;
        file->current_cluster = ctx->root_cluster;
        file->is_dir = true;
        return true;
    }

    uint32_t current_cluster = ctx->root_cluster;
    char component[13];
    const char *p = path;
    fat32_dir_entry_t entry;

    while (*p) {
        while (*p == '/')
            p++;
        if (!*p)
            break;

        size_t i = 0;
        while (*p && *p != '/' && i < sizeof(component) - 1) {
            component[i++] = *p++;
        }
        component[i] = '\0';

        if (!fat32_find_file(ctx, current_cluster, component, &entry)) {
            return false;
        }

        if (entry.attributes & ATTR_DIRECTORY) {
            current_cluster = (entry.first_cluster_high << 16) | entry.first_cluster_low;
        } else {
            file->start_cluster = (entry.first_cluster_high << 16) | entry.first_cluster_low;
            file->current_cluster = file->start_cluster;
            file->size = entry.file_size;
            file->is_dir = false;
            return true;
        }
    }

    file->start_cluster = current_cluster;
    file->current_cluster = current_cluster;
    file->is_dir = true;
    return true;
}

uint32_t fat32_next_cluster(fat32_ctx_t *ctx, uint32_t cluster) {
    uint32_t fat_offset = cluster * 4;
    uint32_t fat_sector = ctx->fat_start + (fat_offset / ctx->bytes_per_sector);
    uint32_t entry_offset = fat_offset % ctx->bytes_per_sector;

    uint8_t sector[512];
    if (!pata_pio_read_sector(fat_sector, sector)) {
        return 0;
    }

    uint32_t next = *(uint32_t *) (sector + entry_offset) & FAT32_MASK;

    if (next >= FAT32_EOC) {
        return 0;
    }

    return next;
}

bool fat32_read_cluster(fat32_ctx_t *ctx, uint32_t cluster, void *buffer) {
    return read_sectors(ctx, cluster_to_lba(ctx, cluster), ctx->sectors_per_cluster, buffer);
}

bool fat32_read(fat32_file_t *file, void *buffer, size_t size, size_t *bytes_read) {
    if (!file || !buffer || file->is_dir)
        return false;

    fat32_ctx_t *ctx = file->ctx;
    uint8_t *buf = (uint8_t *) buffer;
    size_t remaining = size;
    *bytes_read = 0;

    while (remaining > 0) {
        if (file->file_offset >= file->size)
            break;

        uint32_t cluster_offset = file->file_offset % ctx->cluster_size;
        uint32_t bytes_in_cluster = ctx->cluster_size - cluster_offset;
        size_t to_read = (remaining < bytes_in_cluster) ? remaining : bytes_in_cluster;
        if (to_read > file->size - file->file_offset) {
            to_read = file->size - file->file_offset;
        }

        uint8_t cluster_data[ctx->cluster_size];
        if (!fat32_read_cluster(ctx, file->current_cluster, cluster_data)) {
            return false;
        }

        memcpy(buf, cluster_data + cluster_offset, to_read);

        buf += to_read;
        file->file_offset += to_read;
        file->cluster_offset += to_read;
        *bytes_read += to_read;
        remaining -= to_read;

        if (file->cluster_offset >= ctx->cluster_size) {
            file->current_cluster = fat32_next_cluster(ctx, file->current_cluster);
            if (!file->current_cluster)
                break;
            file->cluster_offset = 0;
        }
    }

    return true;
}

bool fat32_seek(fat32_file_t *file, uint32_t offset) {
    if (!file || offset > file->size)
        return false;

    fat32_ctx_t *ctx = file->ctx;
    file->file_offset = offset;

    uint32_t target_cluster = file->start_cluster;
    uint32_t cluster_offset = 0;
    uint32_t position = 0;

    while (position + ctx->cluster_size <= offset) {
        target_cluster = fat32_next_cluster(ctx, target_cluster);
        if (!target_cluster)
            return false;
        position += ctx->cluster_size;
    }

    file->current_cluster = target_cluster;
    file->cluster_offset = offset - position;
    return true;
}

bool fat32_read_dir(fat32_file_t *dir, fat32_dir_entry_t *entry) {
    if (!dir || !dir->is_dir || !entry)
        return false;

    fat32_ctx_t *ctx = dir->ctx;
    uint8_t cluster_data[ctx->cluster_size];

    while (1) {
        if (dir->cluster_offset == 0) {
            if (!fat32_read_cluster(ctx, dir->current_cluster, cluster_data)) {
                return false;
            }
        }

        fat32_dir_entry_t *de = (fat32_dir_entry_t *) (cluster_data + dir->cluster_offset);

        if (de->name[0] == 0) {
            return false;
        }

        dir->cluster_offset += sizeof(fat32_dir_entry_t);
        dir->file_offset += sizeof(fat32_dir_entry_t);

        if (dir->cluster_offset >= ctx->cluster_size) {
            dir->current_cluster = fat32_next_cluster(ctx, dir->current_cluster);
            if (!dir->current_cluster)
                return false;
            dir->cluster_offset = 0;
        }

        if (de->name[0] == 0xE5 || de->attributes == ATTR_LONG_NAME) {
            continue;
        }

        *entry = *de;
        return true;
    }
}

bool fat32_close(fat32_file_t *file) {
    if (!file)
        return false;
    memset(file, 0, sizeof(fat32_file_t));
    return true;
}

bool fat32_mkdir(fat32_ctx_t *ctx, const char *path) {
    char parent[256], dirname[13];
    split_path(path, parent, dirname);

    fat32_file_t parent_dir;
    if (!fat32_open_file(ctx, &parent_dir, parent) || !parent_dir.is_dir)
        return false;

    uint32_t new_cluster = find_free_cluster(ctx);
    if (!new_cluster || !set_fat_entry(ctx, new_cluster, FAT32_EOC))
        return false;

    uint8_t *cluster_data = calloc(1, ctx->cluster_size);
    if (!cluster_data)
        return false;

    fat32_dir_entry_t *dot = (fat32_dir_entry_t *) cluster_data;
    fat32_dir_entry_t *dotdot = (fat32_dir_entry_t *) (cluster_data + sizeof(fat32_dir_entry_t));

    memcpy(dot->name, ".       ", 8);
    memset(dot->name + 8, ' ', 3);
    dot->attributes = ATTR_DIRECTORY;
    dot->first_cluster_high = (new_cluster >> 16) & 0xFFFF;
    dot->first_cluster_low = new_cluster & 0xFFFF;

    memcpy(dotdot->name, "..      ", 8);
    memset(dotdot->name + 8, ' ', 3);
    dotdot->attributes = ATTR_DIRECTORY;
    if (parent_dir.start_cluster == ctx->root_cluster) {
        dotdot->first_cluster_high = (ctx->root_cluster >> 16) & 0xFFFF;
        dotdot->first_cluster_low = ctx->root_cluster & 0xFFFF;
    } else {
        dotdot->first_cluster_high = (parent_dir.start_cluster >> 16) & 0xFFFF;
        dotdot->first_cluster_low = parent_dir.start_cluster & 0xFFFF;
    }

    uint32_t lba = cluster_to_lba(ctx, new_cluster);
    if (!write_sectors(ctx, lba, ctx->sectors_per_cluster, cluster_data)) {
        free(cluster_data);
        return false;
    }
    free(cluster_data);

    bool success = create_direntry(ctx, parent_dir.current_cluster, dirname, ATTR_DIRECTORY, new_cluster, 0);

    fat32_close(&parent_dir);
    return success;
}

bool fat32_create(fat32_ctx_t *ctx, const char *path) {
    char parent[256], filename[13];
    split_path(path, parent, filename);

    fat32_file_t parent_dir;
    if (!fat32_open_file(ctx, &parent_dir, parent) || !parent_dir.is_dir)
        return false;

    bool success = create_direntry(ctx, parent_dir.current_cluster, filename, ATTR_ARCHIVE, 0, 0);

    fat32_close(&parent_dir);
    return success;
}

bool fat32_write(fat32_file_t *file, const void *buffer, size_t size) {
    if (!file || file->is_dir)
        return false;

    fat32_ctx_t *ctx = file->ctx;
    const uint8_t *src = (const uint8_t *) buffer;
    size_t written = 0;

    while (written < size) {
        uint32_t cluster_offset = file->file_offset % ctx->cluster_size;

        if (cluster_offset == 0 && file->file_offset >= file->size) {
            uint32_t new_cluster = find_free_cluster(ctx);
            if (!new_cluster)
                break;

            if (file->current_cluster) {
                set_fat_entry(ctx, file->current_cluster, new_cluster);
            } else {
                file->start_cluster = new_cluster;
            }
            set_fat_entry(ctx, new_cluster, FAT32_EOC);
            file->current_cluster = new_cluster;
        }

        uint8_t *cluster_data = malloc(ctx->cluster_size);
        if (!cluster_data)
            break;
        uint32_t lba = cluster_to_lba(ctx, file->current_cluster);
        if (!read_sectors(ctx, lba, ctx->sectors_per_cluster, cluster_data)) {
            free(cluster_data);
            break;
        }

        size_t to_write = (size - written < ctx->cluster_size - cluster_offset) ? size - written
                                                                                : ctx->cluster_size - cluster_offset;

        memcpy(cluster_data + cluster_offset, src + written, to_write);

        if (!write_sectors(ctx, lba, ctx->sectors_per_cluster, cluster_data)) {
            free(cluster_data);
            break;
        }
        free(cluster_data);

        written += to_write;
        file->file_offset += to_write;
        file->cluster_offset += to_write;
        if (file->file_offset > file->size) {
            file->size = file->file_offset;
        }
    }
    return written == size;
}

bool fat32_rewrite(fat32_file_t *file, const void *buffer, size_t size) {
    if (!file || file->is_dir)
        return false;

    fat32_ctx_t *ctx = file->ctx;
    uint32_t cluster = file->start_cluster;
    while (cluster && cluster < FAT32_EOC) {
        uint32_t next = fat32_next_cluster(ctx, cluster);
        set_fat_entry(ctx, cluster, 0);
        cluster = next;
    }

    file->start_cluster = 0;
    file->current_cluster = 0;
    file->size = 0;
    file->file_offset = 0;
    file->cluster_offset = 0;

    return fat32_write(file, buffer, size);
}

bool fat32_remove(fat32_ctx_t *ctx, const char *path) {
    char parent[256], name[13];
    split_path(path, parent, name);

    fat32_file_t parent_dir;
    fat32_dir_entry_t entry;
    if (!fat32_open_file(ctx, &parent_dir, parent) || !fat32_find_file(ctx, parent_dir.start_cluster, name, &entry))
        return false;

    uint8_t *cluster_data = malloc(ctx->cluster_size);
    if (!cluster_data)
        return false;
    uint32_t lba = cluster_to_lba(ctx, parent_dir.current_cluster);
    if (!read_sectors(ctx, lba, ctx->sectors_per_cluster, cluster_data)) {
        free(cluster_data);
        return false;
    }

    fat32_dir_entry_t *de = (fat32_dir_entry_t *) cluster_data;
    for (uint32_t i = 0; i < ctx->cluster_size / sizeof(fat32_dir_entry_t); i++) {
        if (memcmp(de[i].name, entry.name, 11) == 0) {
            de[i].name[0] = 0xE5;
            bool success = write_sectors(ctx, lba, ctx->sectors_per_cluster, cluster_data);
            free(cluster_data);
            if (success) {
                fat32_update_timestamps(ctx, parent_dir.start_cluster, name, false, false, true);
            }
            return success;
        }
    }

    free(cluster_data);
    return false;
}

bool fat32_rmdir(fat32_ctx_t *ctx, const char *path) {
    fat32_file_t dir;
    if (!fat32_open_file(ctx, &dir, path) || !dir.is_dir)
        return false;

    fat32_dir_entry_t entry;
    while (fat32_read_dir(&dir, &entry)) {
        if (entry.name[0] != '.' && entry.name[0] != 0 && entry.name[0] != 0xE5)
            return false;
    }

    return fat32_remove(ctx, path);
}

bool fat32_update_file_entry(fat32_ctx_t *ctx, const char *path, uint32_t start_cluster, uint32_t size) {
    char parent[256], name[13];
    split_path(path, parent, name);

    fat32_file_t parent_dir;
    if (!fat32_open_file(ctx, &parent_dir, parent) || !parent_dir.is_dir)
        return false;

    uint8_t *cluster_data = malloc(ctx->cluster_size);
    if (!cluster_data) {
        fat32_close(&parent_dir);
        return false;
    }

    uint32_t lba = cluster_to_lba(ctx, parent_dir.start_cluster);
    if (!read_sectors(ctx, lba, ctx->sectors_per_cluster, cluster_data)) {
        free(cluster_data);
        fat32_close(&parent_dir);
        return false;
    }

    fat32_dir_entry_t *de = (fat32_dir_entry_t *) cluster_data;
    bool found = false;
    for (uint32_t i = 0; i < ctx->cluster_size / sizeof(fat32_dir_entry_t); i++) {
        if (de[i].name[0] == 0x00 || de[i].name[0] == 0xE5)
            continue;

        char entry_name[13];
        int j = 0;
        for (int k = 0; k < 8; k++) {
            if (de[i].name[k] == ' ')
                break;
            entry_name[j++] = de[i].name[k];
        }
        if (de[i].name[8] != ' ') {
            entry_name[j++] = '.';
            for (int k = 8; k < 11; k++) {
                if (de[i].name[k] == ' ')
                    break;
                entry_name[j++] = de[i].name[k];
            }
        }
        entry_name[j] = '\0';

        if (strcasecmp(entry_name, name) == 0) {
            de[i].first_cluster_high = (start_cluster >> 16) & 0xFFFF;
            de[i].first_cluster_low = start_cluster & 0xFFFF;
            de[i].file_size = size;

            uint8_t seconds, minutes, hours, day, month, year;
            get_rtc_time(&seconds, &minutes, &hours);
            get_rtc_date(&day, &month, &year);
            de[i].last_write_time = time_to_fat(hours, minutes, seconds);
            de[i].last_write_date = date_to_fat(day, month, year);

            found = true;
            break;
        }
    }

    bool success = false;
    if (found) {
        success = write_sectors(ctx, lba, ctx->sectors_per_cluster, cluster_data);
    }

    free(cluster_data);
    fat32_close(&parent_dir);
    return success;
}
