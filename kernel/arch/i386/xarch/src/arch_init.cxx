#include <arch/init.hxx>
#include <i386/drivers/vga_text.h>
#include <kernel.hxx>

namespace Arch {
    void init() {
        Kernel::set_video_console(i386_Drivers_VgaText_init());

        Kernel::main();
    }
} // namespace Arch
