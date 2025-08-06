#include <core/panic.h>
#include <memory/map.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define LOW_HEAP_START (int) get_kernel_end()
#define LOW_HEAP_END   0x80000
#define ALIGNMENT      16
#define MIN_BLOCK      (sizeof(block_t) * 2)

#define SIZE_MAX ((size_t) -1)

typedef struct block {
    size_t size;
    struct block *next;
    int free;
} block_t;

static block_t *free_list = NULL;

static void heap_init() {
    if (free_list)
        return;

    free_list = (block_t *) LOW_HEAP_START;
    free_list->size = LOW_HEAP_END - LOW_HEAP_START - sizeof(block_t);
    free_list->next = NULL;
    free_list->free = 1;
}

static void split_block(block_t *block, size_t size) {
    if (block->size <= size + sizeof(block_t) + MIN_BLOCK)
        return; // not enough

    block_t *new_block = (block_t *) ((uint8_t *) block + sizeof(block_t) + size);
    new_block->size = block->size - size - sizeof(block_t);
    new_block->free = 1;
    new_block->next = block->next;

    block->size = size;
    block->next = new_block;
}

static void coalesce_blocks() {
    block_t *current = free_list;
    while (current && current->next) {
        if (current->free && current->next->free &&
            (uint8_t *) current + sizeof(block_t) + current->size == (uint8_t *) current->next) {
            current->size += sizeof(block_t) + current->next->size;
            current->next = current->next->next;
        } else {
            current = current->next;
        }
    }
}

void *mallocl(size_t size) {
    if (size == 0)
        return NULL;
    size = (size + ALIGNMENT - 1) & ~(ALIGNMENT - 1);

    heap_init();

    block_t *best = NULL;
    block_t **best_prev = &free_list;

    // first-fit search
    block_t *current = free_list;
    block_t **prev = &free_list;
    while (current) {
        if (current->free && current->size >= size) {
            if (!best || current->size < best->size) { // best-fit
                best = current;
                best_prev = prev;
            }
        }
        prev = &current->next;
        current = current->next;
    }

    if (!best) {
        panic("mallocl: out of low memory");
        return NULL;
    }

    // split block if possible
    if (best->size >= size + sizeof(block_t) + MIN_BLOCK) {
        split_block(best, size);
    }

    best->free = 0;
    *best_prev = best->next;
    return (void *) (best + 1);
}

void *callocl(size_t nmemb, size_t size) {
    if (size != 0 && nmemb > SIZE_MAX / size) {
        panic("callocl: size overflow");
        return NULL;
    }
    size_t total = nmemb * size;
    void *ptr = mallocl(total);
    if (!ptr)
        return NULL;

    memset(ptr, 0, total);
    return ptr;
}

void freel(void *ptr) {
    if (!ptr)
        return;

    block_t *block = ((block_t *) ptr) - 1;
    if (block < (block_t *) LOW_HEAP_START || block >= (block_t *) LOW_HEAP_END) {
        panic("freel: invalid pointer");
        return;
    }

    block->free = 1;
    memset(ptr, 0, block->size); // zero memory

    // add to LIFO
    block->next = free_list;
    free_list = block;

    coalesce_blocks(); // merge free'eded
}
