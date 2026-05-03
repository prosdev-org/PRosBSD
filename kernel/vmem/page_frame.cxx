#include <machine/paging.hxx>
#include <vmem/page_frame.hxx>

namespace VMem {
    PageFrame::PageFrame(const size_t idx) : idx(idx) {
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
