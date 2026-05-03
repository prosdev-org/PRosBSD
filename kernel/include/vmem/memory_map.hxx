#ifndef VMEM_MEMORY_MAP_HXX
#define VMEM_MEMORY_MAP_HXX

#include <libkxx/vector.hxx>
#include <vmem/phys_addr.hxx>

namespace VMem::MemoryMap {
    struct Region {
        PhysAddr start;
        PhysAddr end;
    };

    void init();
    const kxx::Vector<Region> &get_available();
} // namespace VMem::MemoryMap

#endif
