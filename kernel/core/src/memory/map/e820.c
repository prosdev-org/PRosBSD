#include <memory/map/e820.h>
#include <stdlib.h>

#define E820_BUFFER 0xC0005000

e820_entry_t *e820_get_map(size_t *size) {
    const volatile uint32_t *e820 = (volatile uint32_t *) E820_BUFFER;

    *size = e820[0];
    e820_entry_t *map = mallocl(*size * sizeof(e820_entry_t));
    for (size_t i = 0; i < *size; i++) {
        map[i].address = e820[6 * i + 1] + ((uint64_t) e820[6 * i + 2] << 0x20);
        map[i].length = e820[6 * i + 3] + ((uint64_t) e820[6 * i + 4] << 0x20);
        map[i].type = e820[6 * i + 5];
        map[i].acpi3_attributes = e820[6 * i + 6];
    }

    return map;
}

// TODO: sanitize_e820_map()
