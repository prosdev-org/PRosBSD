#include <machine/paging.hxx>
#include <unique/assert.h>
#include <vmem/page_frame.hxx>

namespace VMem {
    PageFrame::PageFrame(const size_t idx) : idx(idx) {
    }

    PageFrame::PageFrame(const PhysAddr phys_addr) :
        idx(phys_addr.value / Machine::Paging::get_page_size()) {
        ASSERT(phys_addr.value % Machine::Paging::get_page_size() == 0);
    }

    PhysAddr PageFrame::get_phys_addr() const {
        return {
                .value = idx * Machine::Paging::get_page_size(),
        };
    }

    size_t PageFrame::get_idx() const {
        return idx;
    }
} // namespace VMem
