#ifndef SYS_PANIC_HXX
#define SYS_PANIC_HXX

#include <libkxx/print.hxx>
#include <machine/cpu.hxx>
#include <unique/noreturn.h>

namespace Sys {
    template<typename... Args>
    NORETURN void panic(Args... args) {
        static bool nested = false;
        if (nested) {
            Machine::Cpu::halt();
        }
        nested = true;

        kxx::println("Kernel panic: ", args...);
        Machine::Cpu::halt();
    }
} // namespace Sys

#endif
