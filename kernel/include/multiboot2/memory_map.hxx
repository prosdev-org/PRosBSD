#ifndef MULTIBOOT2_MEMORY_MAP_HXX
#define MULTIBOOT2_MEMORY_MAP_HXX

#include <stdint.h>

namespace Multiboot2::MemoryMap {
    uintptr_t get_heap_start();
    uintptr_t get_heap_end();
} // namespace Multiboot2::MemoryMap

#endif
