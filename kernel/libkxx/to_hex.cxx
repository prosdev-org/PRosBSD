#include <libkxx/to_hex.hxx>

namespace kxx {
    String to_hex(uint32_t value) {
        String string;
        string.push_back('0');
        string.push_back('x');

        uint32_t divisor = UINT32_MAX / 16 + 1;
        while (divisor != 0) {
            if (const uint32_t digit_idx = value / divisor; digit_idx >= 10) {
                string.push_back(
                        static_cast<char>('a' + digit_idx - 10));
            } else {
                string.push_back(
                        static_cast<char>('0' + value / divisor));
            }

            value %= divisor;
            divisor /= 16;
        }

        return string;
    }

    String to_hex(uint64_t value) {
        String string;
        string.push_back('0');
        string.push_back('x');

        uint64_t divisor = UINT64_MAX / 16 + 1;
        while (divisor != 0) {
            if (const uint32_t digit_idx = value / divisor; digit_idx >= 10) {
                string.push_back(
                        static_cast<char>('a' + digit_idx - 10));
            } else {
                string.push_back(
                        static_cast<char>('0' + value / divisor));
            }

            value %= divisor;
            divisor /= 16;
        }

        return string;
    }
} // namespace kxx
