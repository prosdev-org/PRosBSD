#pragma once

#include <stdint.h>

namespace Arch::I386::Interrupts::Pic8259 {
    void init(uint8_t offset1, uint8_t offset2);
    void mask_all();
    void unmask_all();
    void send_end_of_interrupt(uint8_t idx);
} // namespace Arch::I386::Interrupts::Pic8259
