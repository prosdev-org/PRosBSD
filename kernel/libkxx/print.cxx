#include <libkxx/math.hxx>
#include <libkxx/print.hxx>

namespace kxx {
    void _print_internal(Sys::OutputStream &output_stream, const StringView &string_view) {
        for (size_t i = 0; i < string_view.get_size(); i++) {
            output_stream.write_object(string_view.get(i));
        }
    }

    void _print_internal(Sys::OutputStream &output_stream, const char ch) {
        output_stream.write_object(ch);
    }

    void _print_internal(Sys::OutputStream &output_stream, const int32_t value) {
        if (value < 0) {
            output_stream.write_object('-');
            _print_internal(output_stream, static_cast<uint32_t>(-value));
        } else {
            _print_internal(output_stream, static_cast<uint32_t>(value));
        }
    }

    void _print_internal(Sys::OutputStream &output_stream, uint32_t value) {
        if (value == 0) {
            output_stream.write_object('0');
            return;
        }

        uint32_t divisor = 1000000000; // MAX_U32 ~= 4.29 * (10 ** 9) -> (10 ** 9)
        while (value / divisor == 0) {
            value %= divisor;
            divisor /= 10;
        }

        while (divisor != 0) {
            output_stream.write_object(static_cast<char>('0' + value / divisor));
            value %= divisor;
            divisor /= 10;
        }
    }

    void _print_internal_digits(Sys::OutputStream &output_stream, const uint32_t digits, uint32_t value) {
        ASSERT(digits <= 10);

        uint32_t divisor = Math::pow(10, digits - 1);
        while (divisor != 0) {
            output_stream.write_object(static_cast<char>('0' + value / divisor));
            value %= divisor;
            divisor /= 10;
        }
    }

    void _print_internal(Sys::OutputStream &output_stream, const Sys::Time::Duration time_duration) {
        _print_internal(output_stream, '[');
        _print_internal(output_stream, static_cast<uint32_t>(time_duration.as_seconds()));
        _print_internal(output_stream, '.');
        _print_internal_digits(output_stream, 6, static_cast<uint32_t>(time_duration.as_micros() % 1000000ULL));
        _print_internal(output_stream, ']');
    }
} // namespace kxx
