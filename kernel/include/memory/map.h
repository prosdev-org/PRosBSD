#ifndef MEMORY_MAP_H
#define MEMORY_MAP_H

#define MEMORY_MAP_MAX_SIZE 64

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef enum { MEMORY_FREE, MEMORY_BUSY } memory_t;

typedef struct {
    uint64_t base;
    uint64_t length;
    memory_t type;
} memory_block_t;

void *get_kernel_end();

void mem_map_init();

void *mem_alloc_size(uint64_t size);

void *mem_alloc(uint64_t *size);

void *mem_alloc_largest(uint64_t *size);

bool mem_has_free();

memory_block_t *get_memory_map(size_t *size);

#endif
