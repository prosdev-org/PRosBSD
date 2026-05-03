#include <arch/i386/cpu/gdt.hxx>
#include <arch/i386/interrupts/idt.hxx>
#include <devices/auto_conf.hxx>
#include <machine/init.hxx>
#include <machine/interrupts.hxx>
#include <multiboot2/init.hxx>
#include <sys/kernel.hxx>
#include <vmem/memory_map.hxx>
#include <vmem/page_frame_allocator.hxx>

namespace Machine {
    void init() {
        Interrupts::init();

        Arch::I386::Cpu::Gdt::init();
        Arch::I386::Interrupts::Idt::init();

        Devices::AutoConf::init();

        Multiboot2::init();

        VMem::MemoryMap::init();
        VMem::PageFrameAllocator::init();

        Sys::Kernel::main();
    }
} // namespace Machine
