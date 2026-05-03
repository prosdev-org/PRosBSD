// TODO: this implementation is very temporary. Change it as soon as possible.

#include <multiboot2/memory_map.hxx>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/panic.hxx>

constexpr uintptr_t heap_alignment = 16;

void *malloc(size_t size) {
    static uintptr_t address = Multiboot2::MemoryMap::get_bootstrap_heap_start();

    if (size == 0) {
        Sys::panic("malloc: allocated 0 bytes");
    }

    if (size % heap_alignment != 0) {
        size += heap_alignment - (size % heap_alignment);
    }

    const auto ptr = reinterpret_cast<void *>(address);
    address += size;

    if (address > Multiboot2::MemoryMap::get_bootstrap_heap_end()) {
        Sys::panic("malloc: out of memory");
    }

    // TODO: DEBUG
    memset(ptr, 0xCD, size);
    return ptr;
}

void free(void *ptr) {
    (void) ptr;
}
