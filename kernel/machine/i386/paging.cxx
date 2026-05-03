#include <machine/paging.hxx>

namespace Machine::Paging {
    size_t get_total_pages() {
        return 0x100000;
    }

    size_t get_page_size() {
        return 0x1000;
    }
} // namespace Machine::Paging
