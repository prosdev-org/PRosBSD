#pragma once

#include <stddef.h>

namespace Machine::Paging {
    size_t get_total_pages();
    size_t get_page_size();
} // namespace Machine::Paging
