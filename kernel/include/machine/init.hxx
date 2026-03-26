#ifndef MACHINE_INIT_HXX
#define MACHINE_INIT_HXX

#include <unique/noreturn.h>

namespace Machine {
    /*
     * Initialize the machine and pass control to Kernel_main().
     * Must not be called anywhere except in the Kernel_entry().
     */
    NORETURN void init();
} // namespace Machine

#endif
