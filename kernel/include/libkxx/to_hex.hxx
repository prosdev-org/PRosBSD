#ifndef LIBKXX_TO_HEX_HXX
#define LIBKXX_TO_HEX_HXX

#include <libkxx/string.hxx>

namespace kxx {
    String to_hex(uint32_t value);
    String to_hex(uint64_t value);
    template<typename T>
    String to_hex(const T *ptr);

    template<typename T>
    String to_hex(const T *ptr) {
        return to_hex(static_cast<uintptr_t>(ptr));
    }
} // namespace kxx

#endif
