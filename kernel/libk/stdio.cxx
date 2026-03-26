#include <libkxx/ref.hxx>
#include <stdio.h>
#include <string.h>
#include <sys/kernel.hxx>
#include <sys/output_stream.hxx>

int putchar(const int ch) {
    Sys::OutputStream *output_stream = Sys::Kernel::get_output_stream();
    if (output_stream == nullptr) {
        return -1;
    }

    output_stream->write_object(kxx::Ref<const char>(static_cast<unsigned char>(ch)));
    output_stream->flush();
    return ch;
}

int puts(const char *str) {
    Sys::OutputStream *output_stream = Sys::Kernel::get_output_stream();
    if (output_stream == nullptr) {
        return -1;
    }

    output_stream->write_array(kxx::Ref(str), strlen(str));
    output_stream->write_object(kxx::Ref<const char>('\n'));
    output_stream->flush();
    return 0;
}
