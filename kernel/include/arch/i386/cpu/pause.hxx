#ifndef ARCH_I386_CPU_PAUSE_HXX
#define ARCH_I386_CPU_PAUSE_HXX

namespace Arch::I386::Cpu {
    static inline void pause() {
        __asm__ volatile("pause");
    }
} // namespace Arch::I386::Cpu

#endif
