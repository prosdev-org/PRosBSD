#ifndef KERNEL_H
#define KERNEL_H

#include <console/video_console.h>
#include <unique/public.h>

/*
 * The kernel's main procedure.
 * Must not be called anywhere except in the Arch_init().
 */
PUBLIC noreturn void Kernel_main();

PUBLIC void Kernel_set_video_console(VideoConsole new_video_console);

#endif
