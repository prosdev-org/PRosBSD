#ifndef KERNEL_HXX
#define KERNEL_HXX

#include <output_stream.hxx>

namespace Kernel {
    /*
     * The kernel's main procedure.
     * Must not be called anywhere except in the Arch_init().
     */
    noreturn void main();

    void set_output_stream(OutputStream *new_output_stream);
} // namespace Kernel

#endif
