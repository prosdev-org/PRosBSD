#include <unique/assert.h>
#include <vmem/i386/layout.hxx>
#include <vmem/phys_addr.hxx>

namespace VMem {
    PhysAddr PhysAddr::from_virt(const uintptr_t virt) {
        ASSERT((virt >= I386::Layout::low_map_start) &&
               (virt <= I386::Layout::low_map_end));

        return {
                .value = virt - I386::Layout::low_map_start,
        };
    }

    uintptr_t PhysAddr::to_virt() const {
        ASSERT(value <= (I386::Layout::low_map_end -
                         I386::Layout::low_map_start));

        return value + I386::Layout::low_map_start;
    }
} // namespace VMem
