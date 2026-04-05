#ifndef LIBKXX_PRINT_HXX
#define LIBKXX_PRINT_HXX

#include <libkxx/string.hxx>
#include <sys/kernel.hxx>
#include <sys/output_stream.hxx>

namespace kxx {
    void _print_internal(Sys::OutputStream *output_stream, const String &string);
    void _print_internal(Sys::OutputStream *output_stream, const char *str);
    void _print_internal(Sys::OutputStream *output_stream, char ch);

    template<typename... Args>
    void print(Args... args) {
        Sys::OutputStream *output_stream = Sys::Kernel::get_output_stream();
        if (output_stream == nullptr) {
            return;
        }

        (_print_internal(output_stream, args), ...);

        output_stream->flush();
    }

    template<typename... Args>
    void println(Args... args) {
        Sys::OutputStream *output_stream = Sys::Kernel::get_output_stream();
        if (output_stream == nullptr) {
            return;
        }

        (_print_internal(output_stream, args), ...);

        output_stream->write_object('\n');
        output_stream->flush();
    }
} // namespace kxx

#endif
