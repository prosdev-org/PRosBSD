#include <arch/init.hxx>
#include <i386/drivers/vga_text.hxx>
#include <kernel.hxx>

namespace Arch {
    void init() {
        auto vga_text = I386::Drivers::VgaText(0xC00B8000); // TODO: Drivers registry
        const auto output_stream = vga_text.to_output_stream(); // memory leak

        Kernel::set_output_stream(output_stream);

        Kernel::main();
    }
} // namespace Arch
