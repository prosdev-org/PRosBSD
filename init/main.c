#include <extrns.h>
#include <keyboard.h>
#include <pata_pio.h>
#include <tty.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#pragma pack(push, 1)

typedef struct {
    uint8_t _unused[8];
    uint32_t lba_start;
    uint32_t num_sectors;
} mbr_part_t;

#pragma pack(pop)

int main() {
    keyboard_init();
    cleark();

    printf("Main protected mode kernel is loaded!\n");

    uint8_t sector[512];
    pata_pio_read_sector(0, sector);

    for (int i = 0; i < 512; i++) {
        printf("%d ", sector[i]);
    }
}
