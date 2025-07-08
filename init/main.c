#include <extrns.h>
#include <keyboard.h>
#include <pata_pio.h>
#include <string.h>
#include <tty.h>

#pragma pack(push, 1)
typedef struct {
    uint8_t boot_flag;
    uint8_t chs_start[3];
    uint8_t part_type;
    uint8_t chs_end[3];
    uint32_t lba_start;
    uint32_t num_sectors;
} mbr_part_t;
#pragma pack(pop)

#define SECTOR_SIZE 512

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

    uint8_t mbr[SECTOR_SIZE];
    read_sector(0, mbr);

    mbr_part_t *parts = (mbr_part_t *) (mbr + 0x1BE);

    for (int i = 0; i < 4; i++) {
        uint32_t sec = parts[i].num_sectors;
        if (sec == 0) {
            continue;
        }

        uint64_t bytes = (uint64_t) sec * SECTOR_SIZE;
        printf("Partition %d:\n", i + 1);
        printf("  Type ID   = 0x%d (%s)\n", parts[i].part_type, mbr_type_name(parts[i].part_type));
        printf("  Start LBA = %d\n", parts[i].lba_start);
        printf("  Sectors   = %d\n", sec);
        printf("  Size      = %d bytes\n", bytes);

        printf("  FS probe  = %s\n\n", probe_fs(parts[i].lba_start));
    }

    pata_pio_standby();
    return 0;
}
