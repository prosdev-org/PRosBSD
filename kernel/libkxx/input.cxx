#include <libkxx/input.hxx>
#include <libkxx/print.hxx>
#include <sys/kernel.hxx>

namespace kxx {
    char input_char() {
        Sys::InputStream &input_stream = Sys::Kernel::get_input_stream();

        char ch;
        input_stream.read_object(&ch);
        print(ch);

        return ch;
    }

    String input() {
        Sys::InputStream &input_stream = Sys::Kernel::get_input_stream();

        String string;
        char ch = '\0';
        for (;;) {
            input_stream.read_object(&ch);
            print(ch);

            if (ch == '\n') {
                break;
            }

            string.push_back(ch);
        }

        return string;
    }
} // namespace kxx
