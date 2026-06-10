#pragma once

namespace Arch::I386::Cpu {
    static inline void sti() {
        __asm__ volatile("sti" :);
    }
} // namespace Arch::I386::Cpu
