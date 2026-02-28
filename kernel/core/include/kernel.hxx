#ifndef KERNEL_HXX
#define KERNEL_HXX

#include <console/video_console.h>

namespace Kernel {
    /*
     * The kernel's main procedure.
     * Must not be called anywhere except in the Arch_init().
     */
    noreturn void main();

    void set_video_console(VideoConsole new_video_console);
} // namespace Kernel

#endif
