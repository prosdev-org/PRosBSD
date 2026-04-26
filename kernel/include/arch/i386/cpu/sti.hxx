#ifndef ARCH_I386_CPU_STI_HXX
#define ARCH_I386_CPU_STI_HXX

namespace Arch::I386::Cpu {
    static inline void sti() {
        __asm__ volatile("sti" :);
    }
} // namespace Arch::I386::Cpu

#endif
