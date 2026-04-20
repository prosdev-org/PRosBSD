#include <libkxx/math.hxx>

namespace kxx::Math {
    uint32_t pow(uint32_t base, uint32_t exponent) {
        unsigned result = 1;
        while (exponent != 0) {
            if (exponent & 1) {
                result *= base;
            }

            exponent >>= 1;
            base *= base;
        }

        return result;
    }
} // namespace kxx::Math
