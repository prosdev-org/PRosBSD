#include <libkxx/print.hxx>
#include <string.h>
#include <unique/assert.h>

namespace kxx {
    void _print_internal(Sys::OutputStream *output_stream, const String &string) {
        ASSERT(output_stream != nullptr);

        for (size_t i = 0; i < string.get_size(); i++) {
            output_stream->write_object(string.get(i));
        }
    }

    void _print_internal(Sys::OutputStream *output_stream, const char *str) {
        ASSERT(output_stream != nullptr);

        output_stream->write_array(str, strlen(str));
    }

    void _print_internal(Sys::OutputStream *output_stream, const char ch) {
        ASSERT(output_stream != nullptr);

        output_stream->write_object(ch);
    }
} // namespace kxx
