#include <cmos.h>
#include <extrns.h>
#include <fs/fat32.h>
#include <fs/mbr.h>
#include <keyboard.h>
#include <pata_pio.h>
#include <string.h>
#include <tty.h>

#define SECTOR_SIZE 512

void fat32_format_name(const fat32_dir_entry_t *entry, char *buffer) {
    int i = 0;
    for (; i < 8 && entry->name[i] != ' '; i++) {
        buffer[i] = entry->name[i];
    }

    if (entry->name[8] != ' ') {
        buffer[i++] = '.';
        for (int j = 8; j < 11 && entry->name[j] != ' '; j++) {
            buffer[i++] = entry->name[j];
        }
    }
    buffer[i] = '\0';
}

static const char *mbr_type_name(uint8_t id) {
    switch (id) {
        case 0x00:
            return "Unused";
        case 0x01:
            return "FAT12";
        case 0x04:
            return "FAT16 (<32M)";
        case 0x06:
            return "FAT16";
        case 0x0B:
            return "FAT32 CHS";
        case 0x0C:
            return "FAT32 LBA";
        case 0x07:
            return "NTFS/exFAT";
        case 0x82:
            return "Linux swap";
        case 0x83:
            return "Linux native";
        default:
            return "Unknown";
    }
}

static inline void read_sector(uint32_t lba, void *buf) {
    pata_pio_read_sector(lba, buf);
}

static const char *probe_fs(uint32_t lba_start) {
    uint8_t buf[SECTOR_SIZE];

    read_sector(lba_start, buf);

    if (memcmp(buf + 3, "NTFS    ", 8) == 0) {
        return "NTFS";
    }

    if (memcmp(buf + 0x52, "FAT32   ", 8) == 0) {
        return "FAT32";
    }

    uint16_t fatsz16 = *(uint16_t *) (buf + 0x16);
    if (fatsz16 != 0) {
        return "FAT16";
    }

    read_sector(lba_start + 2, buf);

    uint16_t sb_magic = *(uint16_t *) (buf + 0x38);
    if (sb_magic == 0xEF53) {
        return "EXT2/3/4";
    }

    return "Unknown";
}

int main(void) {
    keyboard_init();
    cleark();

    fat32_ctx_t ctx;
    uint32_t lba_start = 2048;

    if (!fat32_init(&ctx, lba_start)) {
        return -1;
    }

    fat32_file_t file;
    if (!fat32_open_file(&ctx, &file, "/boot/grub/grub.cfg")) {
        return -1;
    }

    const char *data = "GJKHLKJHLKJHLJKHLJKHLJKHLKJHLKJHLKJHLKJHLKJHLKJHLKJHLKJHLKJHLKJHJKHHJJHKJHHLKJHLKKHJHHLJJHLJHLK"
                       "JLHKJHLKJHLJHKLJKHLJKHJHJHLKJHKJHLJKLJKHLJKHLJKJKLJKHJKHJKHJKHKJKJHJKHJKHJLKHLJKHJHKJLHKJLHKLJK"
                       "HJLHKJHKJHKJLKHJHJHKJK";
    size_t len = strlen(data);
    if (!fat32_rewrite(&file, data, len)) {
        fat32_close(&file);
        return -1;
    }

    if (!fat32_update_file_entry(&ctx, "/boot/grub/grub.cfg", file.start_cluster, file.size)) {
        fat32_close(&file);
        return -1;
    }

    fat32_close(&file);
    printf("Writed!");
    return 0;
}
