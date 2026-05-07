#ifndef VMEM_PAGE_FRAME_HXX
#define VMEM_PAGE_FRAME_HXX

#include <stddef.h>
#include <vmem/phys_addr.hxx>

namespace VMem {
    class PageFrame {
    public:
        explicit PageFrame(size_t idx);
        explicit PageFrame(PhysAddr phys_addr);
        [[nodiscard]] PhysAddr get_phys_addr() const;
        [[nodiscard]] size_t get_idx() const;

    private:
        const size_t idx;
    };
} // namespace VMem

#endif
