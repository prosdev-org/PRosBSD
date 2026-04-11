#ifndef LIBKXX_PRINT_HXX
#define LIBKXX_PRINT_HXX

#include <libkxx/string_view.hxx>
#include <stdint.h>
#include <sys/kernel.hxx>
#include <sys/output_stream.hxx>

namespace kxx {
    void _print_internal(Sys::OutputStream &output_stream, const StringView &string_view);
    void _print_internal(Sys::OutputStream &output_stream, char ch);
    void _print_internal(Sys::OutputStream &output_stream, int32_t value);
    void _print_internal(Sys::OutputStream &output_stream, uint32_t value);

    template<typename... Args>
    void print(const Args &...args) {
        Sys::OutputStream &output_stream = Sys::Kernel::get_output_stream();

        (_print_internal(output_stream, args), ...);

        output_stream.flush();
    }

    template<typename... Args>
    void println(const Args &...args) {
        Sys::OutputStream &output_stream = Sys::Kernel::get_output_stream();

        (_print_internal(output_stream, args), ...);

        output_stream.write_object('\n');
        output_stream.flush();
    }
} // namespace kxx

#endif
