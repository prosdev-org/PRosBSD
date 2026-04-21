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
        }

        _print_internal(output_stream, Math::abs(value));
    }

    void _print_internal(Sys::OutputStream &output_stream, uint32_t value) {
        if (value == 0) {
            output_stream.write_object('0');
            return;
        }

        // MAX_U32 ~= 4.29 * (10 ** 9) -> (10 ** 9)
        constexpr uint32_t initial_divisor = Math::ipow(10U, 9U);

        uint32_t divisor = initial_divisor;
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

    void _print_internal(Sys::OutputStream &output_stream, const int64_t value) {
        if (value < 0) {
            output_stream.write_object('-');
        }

        _print_internal(output_stream, Math::abs(value));
    }

    void _print_internal(Sys::OutputStream &output_stream, uint64_t value) {
        if (value == 0) {
            output_stream.write_object('0');
            return;
        }

        // MAX_U64 ~= 1.84 * (10 ** 19) -> (10 ** 19)
        constexpr uint64_t initial_divisor = Math::ipow(10ULL, 19ULL);

        uint64_t divisor = initial_divisor;
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

        uint32_t divisor = Math::ipow(10U, digits - 1U);
        while (divisor != 0) {
            output_stream.write_object(static_cast<char>('0' + value / divisor));
            value %= divisor;
            divisor /= 10;
        }
    }

    void _print_internal_digits(Sys::OutputStream &output_stream, const uint64_t digits, uint64_t value) {
        ASSERT(digits <= 19);

        uint32_t divisor = Math::ipow(10ULL, digits - 1ULL);
        while (divisor != 0) {
            output_stream.write_object(static_cast<char>('0' + value / divisor));
            value %= divisor;
            divisor /= 10;
        }
    }

    void _print_internal(Sys::OutputStream &output_stream, const Sys::Time::Duration time_duration) {
        _print_internal(output_stream, '[');
        _print_internal(output_stream, time_duration.as_seconds());
        _print_internal(output_stream, '.');
        _print_internal_digits(output_stream, 6, time_duration.as_micros());
        _print_internal(output_stream, ']');
    }
} // namespace kxx
