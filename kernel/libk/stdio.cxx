#include <stdio.h>
#include <string.h>
#include <sys/kernel.hxx>
#include <sys/output_stream.hxx>

int putchar(const int ch) {
    Sys::OutputStream *output_stream = Sys::Kernel::get_output_stream();
    if (output_stream == nullptr) {
        return -1;
    }

    output_stream->write_object(ch);
    output_stream->flush();
    return ch;
}

int puts(const char *str) {
    Sys::OutputStream *output_stream = Sys::Kernel::get_output_stream();
    if (output_stream == nullptr) {
        return -1;
    }

    output_stream->write_array(str, strlen(str));
    output_stream->write_object('\n');
    output_stream->flush();
    return 0;
}
