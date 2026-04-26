#ifndef ARCH_I386_CPU_HALT_HXX
#define ARCH_I386_CPU_HALT_HXX

namespace Arch::I386::Cpu {
    static inline void halt() {
        __asm__ volatile("cli" :);
        __asm__ volatile("hlt" :);
    }
} // namespace Arch::I386::Cpu

#endif
