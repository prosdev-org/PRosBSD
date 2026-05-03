#ifndef MULTIBOOT2_MEMORY_MAP_HXX
#define MULTIBOOT2_MEMORY_MAP_HXX

#include <libkxx/vector.hxx>
#include <multiboot2/tags/memory_map.hxx>
#include <stdint.h>

namespace Multiboot2::MemoryMap {
    void init(kxx::Vector<Tags::MemoryMap::Region> &&new_available);
    /*
     * Memory regions are aligned by Machine::Paging::get_page_size()
     */
    const kxx::Vector<Tags::MemoryMap::Region> &get_available();
    uintptr_t get_bootstrap_heap_start();
    uintptr_t get_bootstrap_heap_end();
    uintptr_t get_kernel_end();
} // namespace Multiboot2::MemoryMap

#endif
