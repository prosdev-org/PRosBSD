#ifndef ARCH_INIT_H
#define ARCH_INIT_H

#include <unique/noreturn.h>
#include <unique/public.h>

/*
 * Initialize architecture-specific stuff and pass control to Kernel_main().
 * Must not be called anywhere except in the Kernel_entry().
 */
PUBLIC noreturn void Arch_init();

#endif
