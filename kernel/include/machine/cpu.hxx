#ifndef MACHINE_CPU_HXX
#define MACHINE_CPU_HXX

#include <unique/noreturn.h>

namespace Machine::Cpu {
    NORETURN void halt();
    void relax();
} // namespace Machine::Cpu

#endif
