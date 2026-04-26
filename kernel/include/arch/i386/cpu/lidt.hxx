#ifndef ARCH_I386_CPU_LIDT_HXX
#define ARCH_I386_CPU_LIDT_HXX

#include <stdint.h>

namespace Arch::I386::Cpu {
    static inline void lidt(void *base, uint16_t limit) {
        struct Idtr {
            uint16_t limit;
            uint32_t base;
        } __attribute__((packed));

        Idtr idtr = {
                .limit = limit,
                .base = reinterpret_cast<uint32_t>(base),
        };

        __asm__("lidt %0" ::"m"(idtr));
    }
} // namespace Arch::I386::Cpu

#endif
