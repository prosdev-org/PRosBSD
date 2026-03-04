#include <arch/init.hxx>
#include <i386/drivers/vga_text.hxx>
#include <kernel.hxx>

namespace Arch {
    void init() {
        auto vga_text = I386::Drivers::VgaText(0xC00B8000); // TODO: Drivers registry
        Kernel::set_video_console(&vga_text);

        Kernel::main();
    }
} // namespace Arch
