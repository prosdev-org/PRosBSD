#include <stdio.h>
#include <sys/kernel.hxx>
#include <sys/panic.hxx>
#include <unique/extern_c.h>

EXTERN_C int vprintf_(const char *format, va_list va);

static Sys::OutputStream *output_stream;

EXTERN_C void _putchar(const char ch) {
    if (output_stream == nullptr) {
        Sys::panic("printf: output_stream is nullptr");
    }

    output_stream->write_object(ch);
}

EXTERN_C int printf(const char *format, ...) {
    va_list va;
    va_start(va, format);

    output_stream = Sys::Kernel::get_output_stream();
    const int res = vprintf_(format, va);
    output_stream->flush();
    output_stream = nullptr;

    va_end(va);
    return res;
}
