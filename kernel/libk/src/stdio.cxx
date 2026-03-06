#include <kernel.hxx>
#include <output_stream.hxx>
#include <stdio.h>
#include <string.h>

int putchar(const int ch) {
    OutputStream *output_stream = Kernel::get_output_stream();
    output_stream->write_object(static_cast<unsigned char>(ch));
    output_stream->flush();
    return ch;
}

int puts(const char *str) {
    OutputStream *output_stream = Kernel::get_output_stream();
    output_stream->write_array(str, strlen(str));
    output_stream->flush();
    return 0;
}
