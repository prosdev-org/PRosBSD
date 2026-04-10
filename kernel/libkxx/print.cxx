#include <libkxx/print.hxx>
#include <string.h>
#include <unique/assert.h>

namespace kxx {
    void _print_internal(Sys::OutputStream *output_stream, const StringView &string_view) {
        ASSERT(output_stream != nullptr);

        for (size_t i = 0; i < string_view.get_size(); i++) {
            output_stream->write_object(string_view.get(i));
        }
    }

    void _print_internal(Sys::OutputStream *output_stream, const char ch) {
        ASSERT(output_stream != nullptr);

        output_stream->write_object(ch);
    }

    void _print_internal(Sys::OutputStream *output_stream, const int32_t value) {
        ASSERT(output_stream != nullptr);

        if (value < 0) {
            output_stream->write_object('-');
            _print_internal(output_stream, static_cast<uint32_t>(-value));
        } else {
            _print_internal(output_stream, static_cast<uint32_t>(value));
        }
    }

    void _print_internal(Sys::OutputStream *output_stream, uint32_t value) {
        ASSERT(output_stream != nullptr);

        if (value == 0) {
            output_stream->write_object('0');
            return;
        }

        uint32_t divisor = 1000000000; // MAX_U32 ~= 4.29 * (10 ** 9) -> (10 ** 9)
        while (value / divisor == 0) {
            value %= divisor;
            divisor /= 10;
        }

        while (divisor != 0) {
            output_stream->write_object(static_cast<char>('0' + value / divisor));
            value %= divisor;
            divisor /= 10;
        }
    }
} // namespace kxx
