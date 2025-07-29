#ifndef MBR_H
#define MBR_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint8_t boot_flag;
    uint8_t chs_start[3];
    uint8_t part_type;
    uint8_t chs_end[3];
    uint32_t lba_start;
    uint32_t num_sectors;
} __attribute__((packed)) mbr_part_t;

#endif
