#pragma once

#include <unique/noreturn.h>

namespace Machine::Cpu {
    NORETURN void halt();
    void relax();
} // namespace Machine::Cpu
