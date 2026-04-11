#include <arch/i386/cpu/gdt.hxx>
#include <arch/i386/interrupts/idt.hxx>
#include <drivers/i386/vga_text.hxx>
#include <libkxx/move.hxx>
#include <machine/init.hxx>
#include <machine/interrupts.hxx>
#include <sys/kernel.hxx>

namespace Machine {
    void init() {
        auto vga_text = Drivers::I386::VgaText(0xC00B8000);

        auto output_stream = vga_text.as_output_stream();
        Sys::Kernel::set_output_stream(kxx::move(output_stream));

        Arch::I386::Cpu::Gdt::init();
        Arch::I386::Interrupts::Idt::init();
        Interrupts::init();

        Sys::Kernel::main();
    }
} // namespace Machine
