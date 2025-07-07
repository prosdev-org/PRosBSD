#include <stdbool.h>
#include <stdint.h>

void pata_pio_init();

bool pata_pio_read_sector(uint32_t lba, uint8_t *buffer);

bool pata_pio_write_sector(uint32_t lba, const uint8_t *buffer);
