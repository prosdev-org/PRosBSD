#include <arch/i386/cpu/gdt.hxx>
#include <arch/i386/interrupts/idt.hxx>
#include <devices/auto_conf.hxx>
#include <machine/init.hxx>
#include <machine/interrupts.hxx>
#include <multiboot2/init.hxx>
#include <sys/kernel.hxx>

namespace Machine {
    void init() {
        Interrupts::init();

        Arch::I386::Cpu::Gdt::init();
        Arch::I386::Interrupts::Idt::init();

        Devices::AutoConf::init();

        Multiboot2::init();

        Sys::Kernel::main();
    }
} // namespace Machine
