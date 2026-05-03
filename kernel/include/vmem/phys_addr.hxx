#ifndef VMEM_PHYS_ADDR_HXX
#define VMEM_PHYS_ADDR_HXX

#include <stdint.h>

namespace VMem {
    struct PhysAddr {
        uint64_t value; // TODO: make dyn

        [[nodiscard]] static PhysAddr from_virt(uintptr_t virt);
        [[nodiscard]] uintptr_t to_virt() const;
    };
} // namespace VMem

#endif
