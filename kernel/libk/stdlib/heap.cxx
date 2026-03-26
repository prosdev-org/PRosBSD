// TODO: this implementation is very temporary. Change it as soon as possible.

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/panic.hxx>

#define HEAP_START     0xC0200000
#define HEAP_END       0xC0210000
#define HEAP_ALIGNMENT 16

static uintptr_t address = HEAP_START;

void *malloc(size_t size) {
    if (size == 0) {
        Sys::panic("malloc: allocated 0 bytes");
    }

    if (size % HEAP_ALIGNMENT != 0) {
        size += HEAP_ALIGNMENT - (size % HEAP_ALIGNMENT);
    }

    const auto ptr = reinterpret_cast<void *>(address);
    address += size;

    if (address > HEAP_END) {
        Sys::panic("malloc: out of memory");
    }

    // TODO: DGB
    memset(ptr, 0xCD, size);
    return ptr;
}

void free(void *ptr) {
    (void) ptr;
}
