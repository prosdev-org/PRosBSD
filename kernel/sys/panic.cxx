#include <machine/cpu.hxx>
#include <stdio.h>
#include <sys/panic.hxx>

namespace Sys {
    void panic(const char *msg) {
        static bool nested = false;
        if (nested) {
            Machine::Cpu::halt();
        }
        nested = true;
        // TODO: use print
        puts("Kernel panic:");
        puts(msg);
        Machine::Cpu::halt();
    }
} // namespace Sys
