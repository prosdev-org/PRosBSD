#pragma once

#include <stdint.h>

namespace Arch::I386::Cpu {
    static inline void load_cr3(const uint32_t cr3) {
        __asm__ volatile("mov %0, %%cr3" ::"r"(cr3));
    }
} // namespace Arch::I386::Cpu
