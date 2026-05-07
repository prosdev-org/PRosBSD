#ifndef ARCH_I386_INVLPG_HXX
#define ARCH_I386_INVLPG_HXX

#include <stdint.h>

namespace Arch::I386::Cpu {
    static inline void invlpg(uint32_t address) {
        __asm__ volatile("invlpg %0" ::"m"(address));
    }
} // namespace Arch::I386::Cpu

#endif
