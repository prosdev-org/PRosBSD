#ifndef LIBKXX_HEAP_HXX
#define LIBKXX_HEAP_HXX

#include <stdlib.h>

namespace kxx::Heap {
    inline void *malloc(const size_t size) {
        return ::malloc(size);
    }

    inline void free(void *ptr) {
        ::free(ptr);
    }

    size_t get_allocated();
    size_t get_capacity();
    size_t get_available();
    size_t get_peak_allocated();
    size_t get_peak_request_size();
    void log_info();
} // namespace kxx::Heap

#endif
