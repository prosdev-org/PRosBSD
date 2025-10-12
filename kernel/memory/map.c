#include <core/panic.h>
#include <memory/map.h>
#include <memory/map/e820.h>
#include <memory/virtual/paging.h>
#include <stdint.h>
#include <string.h>

extern uint32_t __kernel_end;

static void *get_kernel_physical_end() {
    return (void *) ((size_t) &__kernel_end - PAGING_FIRST_4MIB_MAPPING_ADDR);
}

void *get_kernel_end() {
    return &__kernel_end;
}

static memory_block_t map[MEMORY_MAP_MAX_SIZE];
static size_t map_size = 0;

static void insert(const memory_block_t *memory_block, const size_t idx) {
    if (map_size == MEMORY_MAP_MAX_SIZE)
        panic("Map: Max size exceeded");

    memmove(&map[idx + 1], &map[idx], (map_size++ - idx) * sizeof(memory_block_t));
    map[idx] = *memory_block;
}

static void append(const memory_block_t *memory_block) {
    insert(memory_block, map_size);
}

static void delete(size_t idx) {
    memmove(&map[idx], &map[idx + 1], (--map_size - idx) * sizeof(memory_block_t));
}

static size_t find_first(const uint64_t address) {
    for (size_t i = 0; i < map_size; i++)
        if ((address >= map[i].base && address <= map[i].base + map[i].length) || address <= map[i].base)
            return i;

    panic("Map: Failed to find memory");
}

static bool split(const uint64_t address, const size_t idx) {
    if (address <= map[idx].base || address >= map[idx].base + map[idx].length)
        return false;

    const memory_block_t new = {
            .base = address, .length = map[idx].base + map[idx].length - address, .type = map[idx].type};

    map[idx].length = address - map[idx].base;

    insert(&new, idx + 1);
    return true;
}

void mem_map_init() {
    size_t e820_size;
    const e820_entry_t *e820_map = e820_get_map(&e820_size);
    for (size_t i = 0; i < e820_size; i++) {
        if (e820_map[i].length == 0 || e820_map[i].type != 1)
            continue;

        memory_block_t memory_block;
        memory_block.base = e820_map[i].address;
        memory_block.length = e820_map[i].length;
        memory_block.type = MEMORY_FREE;

        append(&memory_block);
    }

    // Remove everything lower than kernel end
    for (size_t i = 0; i < find_first((size_t) get_kernel_physical_end()); i++)
        delete (0);

    if (split((size_t) get_kernel_physical_end(), 0))
        delete (0);
}

static void *mem_alloc_common(const size_t *target) {
    map[*target].type = MEMORY_BUSY;
    return (void *) (size_t) map[*target].base;
}

void *mem_alloc_size(const uint64_t size) {
    size_t target = 0;
    while (map[target].type != MEMORY_FREE || map[target].length < size) // first fit
        target++;

    void *ptr = mem_alloc_common(&target);

    if (split(map[target].base + size, target))
        map[target + 1].type = MEMORY_FREE;

    return ptr;
}

void *mem_alloc(uint64_t *size) {
    size_t target = 0;
    while (map[target].type != MEMORY_FREE)
        target++;

    void *ptr = mem_alloc_common(&target);
    *size = map[target].length;
    return ptr;
}

bool mem_has_free() {
    for (size_t i = 0; i < map_size; i++)
        if (map[i].type == MEMORY_FREE)
            return true;

    return false;
}

memory_block_t *get_memory_map(size_t *size) {
    *size = map_size;
    return map;
}
