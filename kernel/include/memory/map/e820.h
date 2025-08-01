#ifndef E820_H
#define E820_H

#include <stddef.h>
#include <stdint.h>

typedef struct e820_entry {
    uint64_t address;
    uint64_t length;
    uint32_t type;
    uint32_t acpi3_attributes;
} __attribute__((packed)) e820_entry_t;

e820_entry_t *e820_get_map(size_t *size);

#endif
