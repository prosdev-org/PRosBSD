#ifndef FAT32_H
#define FAT32_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint8_t jmp[3];
    char oem[8];
    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t fat_count;
    uint16_t root_entries;
    uint16_t total_sectors_16;
    uint8_t media_type;
    uint16_t fat_size_16;
    uint16_t sectors_per_track;
    uint16_t head_count;
    uint32_t hidden_sectors;
    uint32_t total_sectors_32;
    uint32_t fat_size_32;
    uint16_t flags;
    uint16_t version;
    uint32_t root_cluster;
    uint16_t fs_info_sector;
    uint16_t backup_boot_sector;
    uint8_t reserved[12];
    uint8_t drive_number;
    uint8_t nt_flags;
    uint8_t signature;
    uint32_t volume_id;
    char volume_label[11];
    char fs_type[8];
} __attribute__((packed)) fat32_boot_sector_t;

typedef struct {
    uint32_t lead_signature;
    uint8_t reserved1[480];
    uint32_t struct_signature;
    uint32_t free_clusters;
    uint32_t next_free;
    uint8_t reserved2[12];
    uint32_t trail_signature;
} __attribute__((packed)) fat32_fsinfo_t;

typedef struct {
    char name[11];
    uint8_t attributes;
    uint8_t nt_reserved;
    uint8_t creation_time_tenths;
    uint16_t creation_time;
    uint16_t creation_date;
    uint16_t last_access_date;
    uint16_t first_cluster_high;
    uint16_t last_write_time;
    uint16_t last_write_date;
    uint16_t first_cluster_low;
    uint32_t file_size;
} __attribute__((packed)) fat32_dir_entry_t;

typedef struct {
    uint32_t lba_start;
    uint32_t bytes_per_sector;
    uint32_t sectors_per_cluster;
    uint32_t cluster_size;
    uint32_t fat_start;
    uint32_t fat_size;
    uint32_t fat_count;
    uint32_t data_start;
    uint32_t root_cluster;
    uint32_t total_clusters;
    fat32_fsinfo_t fsinfo;
} fat32_ctx_t;

typedef struct {
    fat32_ctx_t *ctx;
    uint32_t start_cluster;
    uint32_t current_cluster;
    uint32_t cluster_offset;
    uint32_t file_offset;
    uint32_t size;
    bool is_dir;
    uint32_t dir_index;
} fat32_file_t;

#define ATTR_READ_ONLY 0x01
#define ATTR_HIDDEN    0x02
#define ATTR_SYSTEM    0x04
#define ATTR_VOLUME_ID 0x08
#define ATTR_DIRECTORY 0x10
#define ATTR_ARCHIVE   0x20
#define ATTR_LONG_NAME 0x0F

#define FAT32_EOC  0x0FFFFFF8
#define FAT32_MASK 0x0FFFFFFF

bool read_sectors(fat32_ctx_t *ctx, uint32_t lba, uint32_t count, void *buffer);
bool write_sectors(fat32_ctx_t *ctx, uint32_t lba, uint32_t count, void *buffer);

uint32_t cluster_to_lba(fat32_ctx_t *ctx, uint32_t cluster);

uint16_t time_to_fat(uint8_t hours, uint8_t minutes, uint8_t seconds);
uint16_t date_to_fat(uint8_t day, uint8_t month, uint8_t year);

bool fat32_init(fat32_ctx_t *ctx, uint32_t lba_start);
bool fat32_update_timestamps(fat32_ctx_t *ctx, uint32_t dir_cluster, const char *name, bool update_creation,
                             bool update_access, bool update_modification);
bool fat32_open_root(fat32_ctx_t *ctx, fat32_file_t *file);
void split_path(const char *path, char *parent, char *name);
uint32_t find_free_cluster(fat32_ctx_t *ctx);
bool set_fat_entry(fat32_ctx_t *ctx, uint32_t cluster, uint32_t value);
bool create_direntry(fat32_ctx_t *ctx, uint32_t dir_cluster, const char *name, uint8_t attrs, uint32_t first_cluster,
                     uint32_t size);
bool fat32_find_file(fat32_ctx_t *ctx, uint32_t start_cluster, const char *name, fat32_dir_entry_t *entry);
bool fat32_open_file(fat32_ctx_t *ctx, fat32_file_t *file, const char *path);
uint32_t fat32_next_cluster(fat32_ctx_t *ctx, uint32_t cluster);
bool fat32_read_cluster(fat32_ctx_t *ctx, uint32_t cluster, void *buffer);
bool fat32_read(fat32_file_t *file, void *buffer, size_t size, size_t *bytes_read);
bool fat32_seek(fat32_file_t *file, uint32_t offset);
bool fat32_read_dir(fat32_file_t *dir, fat32_dir_entry_t *entry);
bool fat32_close(fat32_file_t *file);
bool fat32_mkdir(fat32_ctx_t *ctx, const char *path);
bool fat32_create(fat32_ctx_t *ctx, const char *path);
bool fat32_write(fat32_file_t *file, const void *buffer, size_t size);
bool fat32_rewrite(fat32_file_t *file, const void *buffer, size_t size);
bool fat32_remove(fat32_ctx_t *ctx, const char *path);
bool fat32_rmdir(fat32_ctx_t *ctx, const char *path);
bool fat32_update_file_entry(fat32_ctx_t *ctx, const char *path, uint32_t start_cluster, uint32_t size);

#endif
