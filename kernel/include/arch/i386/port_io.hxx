#ifndef ARCH_I386_PORT_IO_HXX
#define ARCH_I386_PORT_IO_HXX

#include <stdint.h>

namespace Arch::I386::PortIO {
    static inline void out(const uint8_t value, const uint16_t port) {
        __asm__ volatile("outb %0, %1" ::"a"(value), "d"(port));
    }

    static inline uint8_t in(const uint16_t port) {
        uint8_t result;
        __asm__ volatile("inb %1, %0" : "=a"(result) : "d"(port));
        return result;
    }

    static inline void wait() {
        out(0, 0x80);
    }
} // namespace Arch::I386::PortIO

#endif
