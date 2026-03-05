#include <arch/init.hxx>
#include <kernel.hxx>
#include <string.h>
#include <unique/extern_c.h>
#include <unique/noreturn.h>

EXTERN_C noreturn void Kernel_entry() {
    Arch::init();
}

namespace Kernel {
    static OutputStream *output_stream;

    void main() {
        // Temp demo
        constexpr auto msg = "hello, world!";
        const auto msg_len = strlen(msg);
        for (size_t i = 0; i < 32; i++) {
            output_stream->write_array(msg, msg_len);
        }
        output_stream->flush();

        for (;;)
            ;
    }

    void set_output_stream(OutputStream *new_output_stream) {
        output_stream = new_output_stream;
    }
} // namespace Kernel
