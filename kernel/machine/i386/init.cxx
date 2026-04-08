#include <arch/i386/cpu/gdt.hxx>
#include <arch/i386/interrupts/idt.hxx>
#include <drivers/i386/vga_text.hxx>
#include <libkxx/print.hxx>
#include <machine/init.hxx>
#include <sys/kernel.hxx>

namespace Machine {
    void init() {
        auto vga_text = Drivers::I386::VgaText(0xC00B8000); // TODO: Drivers registry
        const auto output_stream = vga_text.to_output_stream(); // memory leak

        Sys::Kernel::set_output_stream(output_stream);

        Arch::I386::Cpu::Gdt::init();
        Arch::I386::Interrupts::Idt::init();

        Sys::Kernel::main();
    }
} // namespace Machine
