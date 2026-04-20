#include <arch/i386/cpu/halt.hxx>
#include <arch/i386/cpu/pause.hxx>
#include <machine/cpu.hxx>

namespace Machine::Cpu {
    void halt() {
        Arch::I386::Cpu::halt();
        for (;;)
            ;
    }

    void relax() {
        Arch::I386::Cpu::pause();
    }
} // namespace Machine::Cpu
