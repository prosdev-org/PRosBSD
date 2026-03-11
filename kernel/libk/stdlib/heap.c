// TODO: this implementation is very temporary. Change it as soon as possible.

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#define HEAP_START     0xC0200000
#define HEAP_END       0xC0210000
#define HEAP_ALIGNMENT 16

static uintptr_t address = HEAP_START;

void *malloc(size_t size) {
    if (size == 0) {
        return NULL;
    }

    if (size % HEAP_ALIGNMENT != 0) {
        size += HEAP_ALIGNMENT - (size % HEAP_ALIGNMENT);
    }

    void *ptr = (void *) address;
    address += size;

    if (address > HEAP_END) {
        return NULL;
    }

    return ptr;
}

void free(void *ptr) {
    (void) ptr;
}
