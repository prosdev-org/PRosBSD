#include <arch/init.hxx>
#include <kernel.hxx>
#include <stdio.h>
#include <unique/extern_c.h>

EXTERN_C noreturn void Kernel_entry() {
    Arch::init();
}

namespace Kernel {
    static OutputStream *output_stream;

    void main() {
        // Temp demo
        for (size_t i = 0; i < 32; i++) {
            puts("Hello, PRosBSD!");
        }

        for (;;)
            ;
    }

    void set_output_stream(OutputStream *new_output_stream) {
        delete output_stream;
        output_stream = new_output_stream;
    }

    OutputStream *get_output_stream() {
        return output_stream;
    }
} // namespace Kernel
