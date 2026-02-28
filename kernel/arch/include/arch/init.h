#ifndef ARCH_INIT_H
#define ARCH_INIT_H

#include <unique/extern_c.h>
#include <unique/noreturn.h>

/*
 * Initialize architecture-specific stuff and pass control to Kernel_main().
 * Must not be called anywhere except in the Kernel_entry().
 */
EXTERN_C noreturn void Arch_init();

#endif
