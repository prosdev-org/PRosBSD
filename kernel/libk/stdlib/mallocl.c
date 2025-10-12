#include <core/panic.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#define LOW_HEAP_START 0xC000B000
#define LOW_HEAP_END   0xC0080000

static uint32_t low_mem_ptr = LOW_HEAP_START;

void *mallocl(const size_t size) {
    low_mem_ptr = (low_mem_ptr + 15) & ~0xF;

    if (low_mem_ptr + size > LOW_HEAP_END) {
        panic("mallocl: out of low memory");
    }

    void *ptr = (void *) low_mem_ptr;
    low_mem_ptr += size;
    return ptr;
}
