#include <arch/i386/cpu/gdt.hxx>
#include <drivers/i386/vga_text.hxx>
#include <machine/init.hxx>
#include <sys/kernel.hxx>

namespace Machine {
    void init() {
        Arch::I386::Cpu::Gdt::init();

        auto vga_text = Drivers::I386::VgaText(0xC00B8000); // TODO: Drivers registry
        const auto output_stream = vga_text.to_output_stream(); // memory leak

        Sys::Kernel::set_output_stream(output_stream);

        Sys::Kernel::main();
    }
} // namespace Machine
