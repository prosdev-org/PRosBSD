#include <arch/i386/cpu/gdt.hxx>
#include <arch/i386/interrupts/idt.hxx>
#include <devices/auto_conf.hxx>
#include <machine/init.hxx>
#include <machine/interrupts.hxx>
#include <multiboot2/init.hxx>
#include <sys/kernel.hxx>
#include <unique/log.hxx>
#include <vmem/memory_map.hxx>
#include <vmem/page_frame_allocator.hxx>

namespace Machine {
    kxx::StringView get_logger_prefix() {
        return "machine";
    }

    void init() {
        Interrupts::init();

        LOG("initializing GDT");
        Arch::I386::Cpu::Gdt::init();
        LOG("initializing IDT");
        Arch::I386::Interrupts::Idt::init();

        Devices::AutoConf::init();

        Multiboot2::init();

        VMem::MemoryMap::init();
        VMem::PageFrameAllocator::init();

        LOG("passing control to Kernel::main()");
        Sys::Kernel::main();
    }
} // namespace Machine
