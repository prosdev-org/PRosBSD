#pragma once

#include <unique/noreturn.h>

namespace Machine {
    /*
     * Initialize the machine and pass control to Kernel_main().
     * Must not be called anywhere except in the Kernel_entry().
     */
    NORETURN void init();
} // namespace Machine
