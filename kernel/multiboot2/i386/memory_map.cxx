#include <multiboot2/memory_map.hxx>
#include <unique/extern_c.h>

namespace Multiboot2::MemoryMap {
    EXTERN_C int __heap_start;
    EXTERN_C int __heap_end;

    uintptr_t get_heap_start() {
        return reinterpret_cast<uintptr_t>(&__heap_start);
    }

    uintptr_t get_heap_end() {
        return reinterpret_cast<uintptr_t>(&__heap_end);
    }
} // namespace Multiboot2::MemoryMap
