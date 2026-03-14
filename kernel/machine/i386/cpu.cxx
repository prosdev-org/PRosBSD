#include <arch/i386/cpu/halt.hxx>
#include <machine/cpu.hxx>

namespace Machine::Cpu {
    void halt() {
        Arch::I386::Cpu::halt();
        for (;;)
            ;
    }
} // namespace Machine::Cpu
