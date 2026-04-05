#include <libkxx/print.hxx>
#include <string.h>
#include <unique/assert.h>

namespace kxx {
    void _print_internal(Sys::OutputStream *output_stream, const char *str) {
        ASSERT(output_stream != nullptr);

        output_stream->write_array(str, strlen(str));
    }
} // namespace kxx
