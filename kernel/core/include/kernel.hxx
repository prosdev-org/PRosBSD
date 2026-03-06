#ifndef KERNEL_HXX
#define KERNEL_HXX

#include <output_stream.hxx>
#include <unique/noreturn.h>

namespace Kernel {
    /*
     * The kernel's main procedure.
     * Must not be called anywhere except in the Arch_init().
     */
    noreturn void main();

    /*
     * Set the kernel's OutputStream.
     * Takes ownership
     */
    void set_output_stream(OutputStream *new_output_stream);

    OutputStream *get_output_stream();
} // namespace Kernel

#endif
