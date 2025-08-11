#include <core/panic.h>
#include <memory/map.h>
#include <stdbool.h>

#define PFA_MEMORY_MAP_MAX_SIZE MEMORY_MAP_MAX_SIZE
#define BITMAP_SIZE             0x100000 / 32

typedef struct {
    uint32_t base;
    uint32_t length;
} pfa_memory_t;

static pfa_memory_t map[PFA_MEMORY_MAP_MAX_SIZE];
static size_t map_size = 0;

static uint32_t *bitmap; // 0 - free, 1 - busy
static uint32_t last_alloc = 0;

void pfa_init() {
    bitmap = mem_alloc_size(BITMAP_SIZE * sizeof(uint32_t));
    for (size_t i = 0; i < BITMAP_SIZE; i++)
        bitmap[i] = 0;

    while (mem_has_free()) {
        uint64_t length;
        uint64_t base = (size_t) mem_alloc(&length);

        if (base > 0xFFFFFFFF)
            break;

        if (length < 0x1000)
            continue;

        if (base + length > 0x100000000)
            length = 0x100000000 - base;

        if (base & 0xFFF) {
            length -= 0x1000 - (base & 0xFFF);
            base &= 0xFFFFF000;
            base += 0x1000;
        }

        length &= 0xFFFFF000;

        if (length == 0)
            continue;

        if (map_size == PFA_MEMORY_MAP_MAX_SIZE)
            panic("PFA: Memory map size exceeded");

        map[map_size].base = (uint32_t) base;
        map[map_size++].length = (uint32_t) length;
    }
}

static bool is_usable(uint32_t address) {
    for (size_t i = 0; i < map_size; i++)
        if (address >= map[i].base && address < map[i].base + map[i].length)
            return true;

    return false;
}

static bool is_free_and_calc_idxs(uint32_t idx, size_t *sup_idx, size_t *sub_idx) {
    *sup_idx = idx >> 5;
    *sub_idx = idx & 0x1F;

    return !((bitmap[*sup_idx] & (1u << *sub_idx)) >> *sub_idx);
}

static uint32_t pf_alloc_general(uint32_t start, uint32_t end, bool *found) {
    for (size_t i = start; i < end; i++) {
        if (!is_usable(i << 12))
            continue;

        size_t sup_idx;
        size_t sub_idx;

        if (is_free_and_calc_idxs(i, &sup_idx, &sub_idx)) {
            bitmap[sup_idx] |= 1u << sub_idx;
            *found = true;
            last_alloc = i;
            return i;
        }
    }

    *found = false;
    return 0;
}

uint32_t pf_alloc() {
    bool found;
    uint32_t idx = pf_alloc_general(last_alloc, BITMAP_SIZE * sizeof(uint32_t), &found);
    if (found)
        return idx;
    else {
        idx = pf_alloc_general(0, last_alloc, &found);
        if (found)
            return idx;
    }

    panic("PFA: Ran out of page frames");
}

void pf_free(uint32_t idx) {
    if (!is_usable(idx << 12))
        panic("PFA: Attempt to free reserved memory");

    size_t sup_idx;
    size_t sub_idx;

    if (!is_free_and_calc_idxs(idx, &sup_idx, &sub_idx))
        bitmap[sup_idx] &= ~(1u << sub_idx);
    else
        panic("PFA: Double freeing");
}
