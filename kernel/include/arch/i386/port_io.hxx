#ifndef ARCH_I386_PORT_IO_HXX
#define ARCH_I386_PORT_IO_HXX

#include <stdint.h>

namespace Arch::I386 {
    static inline void out(const uint8_t value, const uint16_t port) {
        __asm__ volatile("outb %0, %1" ::"a"(value), "d"(port));
    }
} // namespace Arch::I386

#endif
