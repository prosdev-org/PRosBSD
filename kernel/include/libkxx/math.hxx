#ifndef LIBKXX_MATH_HXX
#define LIBKXX_MATH_HXX

#include <stdint.h>

namespace kxx::Math {
    constexpr uint32_t ipow(uint32_t base, uint32_t exponent) {
        uint32_t result = 1;
        while (exponent != 0) {
            if (exponent & 1) {
                result *= base;
            }

            exponent >>= 1;
            base *= base;
        }

        return result;
    }

    constexpr uint64_t ipow(uint64_t base, uint64_t exponent) {
        uint64_t result = 1;
        while (exponent != 0) {
            if (exponent & 1) {
                result *= base;
            }

            exponent >>= 1;
            base *= base;
        }

        return result;
    }

    constexpr uint32_t abs(const int32_t value) {
        if (value < 0) {
            return -value;
        }

        return value;
    }

    constexpr uint64_t abs(const int64_t value) {
        if (value < 0) {
            return -value;
        }

        return value;
    }

    template<typename T>
    constexpr T max(const T val1, const T val2) {
        if (val1 > val2) {
            return val1;
        } else {
            return val2;
        }
    }

    template<typename T>
    constexpr T min(const T val1, const T val2) {
        if (val1 < val2) {
            return val1;
        } else {
            return val2;
        }
    }
} // namespace kxx::Math

#endif
