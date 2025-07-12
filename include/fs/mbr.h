#include <stddef.h>
#include <stdint.h>

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
