#include <arch/init.h>
#include <i386/drivers/vga_text.h>
#include <kernel.h>

void Arch_init() {
    Kernel_set_video_console(i386_Drivers_VgaText_init());

    Kernel_main();
}
