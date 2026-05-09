#include <libkxx/math.hxx>
#include <libkxx/print.hxx>
#include <libkxx/to_hex.hxx>

namespace kxx {
    void _print_internal(Sys::OutputStream &output_stream, const StringView &string_view) {
        for (size_t i = 0; i < string_view.get_size(); i++) {
            output_stream.write_object(string_view.get(i));
        }
    }

    void _print_internal(Sys::OutputStream &output_stream, const char ch) {
        output_stream.write_object(ch);
    }

    void _print_internal(Sys::OutputStream &output_stream, const int value) {
        if (value < 0) {
            output_stream.write_object('-');
        }

        _print_internal(output_stream, Math::abs(value));
    }

    void _print_internal(Sys::OutputStream &output_stream, unsigned int value) {
        if (value == 0) {
            output_stream.write_object('0');
            return;
        }

        unsigned int divisor = Math::ipow(10, Math::ilog(10, value));

        while (divisor != 0) {
            output_stream.write_object(static_cast<char>('0' + value / divisor));
            value %= divisor;
            divisor /= 10;
        }
    }

    void _print_internal(Sys::OutputStream &output_stream, const long value) {
        if (value < 0) {
            output_stream.write_object('-');
        }

        _print_internal(output_stream, Math::abs(value));
    }

    void _print_internal(Sys::OutputStream &output_stream, unsigned long value) {
        if (value == 0) {
            output_stream.write_object('0');
            return;
        }

        unsigned int divisor = Math::ipow(10UL, Math::ilog(10UL, value));

        while (divisor != 0) {
            output_stream.write_object(static_cast<char>('0' + value / divisor));
            value %= divisor;
            divisor /= 10;
        }
    }

    void _print_internal(Sys::OutputStream &output_stream, const long long value) {
        if (value < 0) {
            output_stream.write_object('-');
        }

        _print_internal(output_stream, Math::abs(value));
    }

    void _print_internal(Sys::OutputStream &output_stream, unsigned long long value) {
        if (value == 0) {
            output_stream.write_object('0');
            return;
        }

        unsigned int divisor = Math::ipow(10ULL, Math::ilog(10ULL, value));

        while (divisor != 0) {
            output_stream.write_object(static_cast<char>('0' + value / divisor));
            value %= divisor;
            divisor /= 10;
        }
    }

    void _print_internal_digits(Sys::OutputStream &output_stream, const uint32_t digits, uint32_t value) {
        ASSERT(digits <= 10);

        uint32_t divisor = Math::ipow(10U, digits - 1U);
        if (digits < 10) {
            value %= divisor * 10;
        }

        while (divisor != 0) {
            output_stream.write_object(static_cast<char>('0' + value / divisor));
            value %= divisor;
            divisor /= 10;
        }
    }

    void _print_internal_digits(Sys::OutputStream &output_stream, const uint64_t digits, uint64_t value) {
        ASSERT(digits <= 19);

        uint32_t divisor = Math::ipow(10ULL, digits - 1ULL);
        if (digits < 19) {
            value %= divisor * 10;
        }

        while (divisor != 0) {
            output_stream.write_object(static_cast<char>('0' + value / divisor));
            value %= divisor;
            divisor /= 10;
        }
    }

    void _print_internal(Sys::OutputStream &output_stream, void *ptr) {
        _print_internal(output_stream,
                        to_hex(reinterpret_cast<uintptr_t>(ptr)));
    }

    void _print_internal(Sys::OutputStream &output_stream, const Sys::Time::Duration time_duration) {
        _print_internal(output_stream, '[');
        _print_internal(output_stream, time_duration.as_seconds());
        _print_internal(output_stream, '.');
        _print_internal_digits(output_stream, 6, time_duration.as_micros());
        _print_internal(output_stream, ']');
    }
} // namespace kxx
