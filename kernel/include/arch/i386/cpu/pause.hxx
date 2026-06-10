#pragma once

namespace Arch::I386::Cpu {
    static inline void pause() {
        __asm__ volatile("pause");
    }
} // namespace Arch::I386::Cpu
