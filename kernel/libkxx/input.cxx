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
            bool should_print = true;
            const bool backspacing = ch == '\b';

            if (backspacing) {
                if (string.get_size() == 0) {
                    should_print = false;
                } else {
                    string.pop_back();
                }
            }

            if (should_print) {
                print(ch);
            }

            if (backspacing) {
                continue;
            }

            if (ch == '\n') {
                break;
            }

            string.push_back(ch);
        }

        return string;
    }
} // namespace kxx
