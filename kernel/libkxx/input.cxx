#include <libkxx/input.hxx>
#include <sys/kernel.hxx>

namespace kxx {
    char input() {
        Sys::InputStream &input_stream = Sys::Kernel::get_input_stream();

        char ch;
        input_stream.read_object(&ch);
        return ch;
    }
} // namespace kxx
