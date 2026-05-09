#ifndef LIBKXX_MATH_HXX
#define LIBKXX_MATH_HXX

#include <limits.h>
#include <unique/assert.h>

namespace kxx::Math {
    constexpr unsigned int ipow(unsigned int base, unsigned int exp) {
        unsigned int result = 1;
        while (exp != 0) {
            if ((exp & 1) == 1) {
                result *= base;
            }

            exp /= 2;
            base *= base;
        }

        return result;
    }

    constexpr unsigned long ipow(unsigned long base, unsigned long exp) {
        unsigned long result = 1;
        while (exp != 0) {
            if ((exp & 1) == 1) {
                result *= base;
            }

            exp /= 2;
            base *= base;
        }

        return result;
    }

    constexpr unsigned long long ipow(
            unsigned long long base, unsigned long long exp) {
        unsigned long long result = 1;
        while (exp != 0) {
            if ((exp & 1) == 1) {
                result *= base;
            }

            exp /= 2;
            base *= base;
        }

        return result;
    }

    constexpr unsigned int ilog2(const unsigned int value) {
        ASSERT(value != 0);

        return (sizeof(unsigned int) * 8 - 1) - __builtin_clz(value);
    }

    constexpr unsigned long ilog2(const unsigned long value) {
        ASSERT(value != 0);

        return (sizeof(unsigned long) * 8 - 1) - __builtin_clzl(value);
    }

    constexpr unsigned long long ilog2(const unsigned long long value) {
        ASSERT(value != 0);

        return (sizeof(unsigned long long) * 8 - 1) - __builtin_clzll(value);
    }

    constexpr unsigned int ilog(
            const unsigned int base, const unsigned int value) {
        ASSERT(base >= 2);
        ASSERT(value != 0);

        unsigned int result = ilog2(value) / (ilog2(base) + 1);

        while ((ipow(base, result) <= UINT_MAX / base) &&
               ipow(base, result + 1) <= value) {
            result++;
        }

        return result;
    }

    constexpr unsigned long ilog(
            const unsigned long base, const unsigned long value) {
        ASSERT(base >= 2);
        ASSERT(value != 0);

        unsigned long result = ilog2(value) / (ilog2(base) + 1);

        while ((ipow(base, result) <= UINT_MAX / base) &&
               ipow(base, result + 1) <= value) {
            result++;
        }

        return result;
    }

    constexpr unsigned long long ilog(
            const unsigned long long base, const unsigned long long value) {
        ASSERT(base >= 2);
        ASSERT(value != 0);

        unsigned long long result = ilog2(value) / (ilog2(base) + 1);

        while ((ipow(base, result) <= UINT_MAX / base) &&
               ipow(base, result + 1) <= value) {
            result++;
        }

        return result;
    }

    constexpr unsigned int abs(const int value) {
        if (value < 0) {
            return -value;
        }

        return value;
    }

    constexpr unsigned long abs(const long value) {
        if (value < 0) {
            return -value;
        }

        return value;
    }

    constexpr unsigned long long abs(const long long value) {
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
