#ifndef ARCH_INIT_HXX
#define ARCH_INIT_HXX

#include <unique/noreturn.h>

namespace Arch {
    /*
     * Initialize architecture-specific stuff and pass control to Kernel_main().
     * Must not be called anywhere except in the Kernel_entry().
     */
    noreturn void init();
} // namespace Arch

#endif
