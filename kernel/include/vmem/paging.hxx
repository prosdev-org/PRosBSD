#ifndef VMEM_PAGING_HXX
#define VMEM_PAGING_HXX

#include <vmem/phys_addr.hxx>

namespace VMem::Paging {
    void init();
    void map(PhysAddr phys_addr, uintptr_t virt_addr);
    void unmap(uintptr_t virt_addr);
} // namespace VMem::Paging

#endif
