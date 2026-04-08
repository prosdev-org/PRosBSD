#include <arch/i386/interrupts/pic8259.hxx>
#include <arch/i386/port_io.hxx>
#include <stdint.h>

namespace Arch::I386::Interrupts::Pic8259 {
    constexpr uint16_t pic1_port = 0x20;
    constexpr uint16_t pic2_port = 0xA0;
    constexpr uint16_t pic1_data_port = pic1_port + 1;
    constexpr uint16_t pic2_data_port = pic2_port + 1;

    void mask_all() {
        out(0xFF, pic1_data_port);
        out(0xFF, pic2_data_port);
    }

} // namespace Arch::I386::Interrupts::Pic8259
