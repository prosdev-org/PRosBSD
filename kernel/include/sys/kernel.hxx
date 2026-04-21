#ifndef SYS_KERNEL_HXX
#define SYS_KERNEL_HXX

#include <libkxx/unique_ptr.hxx>
#include <sys/input_stream.hxx>
#include <sys/output_stream.hxx>
#include <sys/timer.hxx>
#include <unique/noreturn.h>

namespace Sys::Kernel {
    /*
     * The kernel's main procedure.
     * Must not be called anywhere except in the Arch_init().
     */
    NORETURN void main();

    void set_output_stream(kxx::UniquePtr<OutputStream> &&new_output_stream);
    OutputStream &get_output_stream();

    void set_input_stream(kxx::UniquePtr<InputStream> &&new_input_stream);
    InputStream &get_input_stream();

    void set_timer(kxx::UniquePtr<Timer> &&new_timer);
    Timer &get_timer();
} // namespace Sys::Kernel

#endif
