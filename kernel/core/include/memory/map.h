#ifndef MEMORY_MAP_H
#define MEMORY_MAP_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define MEMORY_MAP_MAX_SIZE 64

typedef enum { MEMORY_FREE, MEMORY_BUSY } memory_t;

typedef struct {
    uint64_t base;
    uint64_t length;
    memory_t type;
} memory_block_t;

void *get_kernel_end();

void mem_map_init();

uintptr_t mem_alloc_size(uint64_t size);

uintptr_t mem_alloc(uint64_t *size);

bool mem_has_free();

memory_block_t *get_memory_map(size_t *size);

#endif
