#include <arch/i386/interrupts/pic8259.hxx>
#include <arch/i386/port_io.hxx>

namespace Arch::I386::Interrupts::Pic8259 {
    constexpr uint16_t pic1_port = 0x20;
    constexpr uint16_t pic1_command_port = pic1_port;
    constexpr uint16_t pic1_data_port = pic1_port + 1;

    constexpr uint16_t pic2_port = 0xA0;
    constexpr uint16_t pic2_command_port = pic2_port;
    constexpr uint16_t pic2_data_port = pic2_port + 1;

    void init(const uint8_t offset1, const uint8_t offset2) {
        constexpr uint8_t icw1_icw4 = 0x01;
        // constexpr uint8_t icw1_single = 0x02;
        // constexpr uint8_t icw1_interval4 = 0x04;
        // constexpr uint8_t icw1_level = 0x08;
        constexpr uint8_t icw1_init = 0x10;

        constexpr uint8_t icw4_8086 = 0x01;
        // constexpr uint8_t icw4_auto = 0x02;
        // constexpr uint8_t icw4_buffered_slave = 0x08;
        // constexpr uint8_t icw4_buffered_master = 0x0C;
        // constexpr uint8_t icw4_sfnm = 0x10;

        constexpr uint8_t cascade_irq = 2;

        // ICW1
        PortIO::out(icw1_init | icw1_icw4, pic1_command_port);
        PortIO::wait();
        PortIO::out(icw1_init | icw1_icw4, pic2_command_port);
        PortIO::wait();

        // ICW2
        PortIO::out(offset1, pic1_data_port);
        PortIO::wait();
        PortIO::out(offset2, pic2_data_port);
        PortIO::wait();

        // ICW3
        PortIO::out(1 << cascade_irq, pic1_data_port);
        PortIO::wait();
        PortIO::out(2, pic2_data_port);
        PortIO::wait();

        // ICW4
        PortIO::out(icw4_8086, pic1_data_port);
        PortIO::wait();
        PortIO::out(icw4_8086, pic2_data_port);
        PortIO::wait();

        unmask_all();
    }

    void mask_all() {
        PortIO::out(0xFF, pic1_data_port);
        PortIO::out(0xFF, pic2_data_port);
    }

    void unmask_all() {
        PortIO::out(0, pic1_data_port);
        PortIO::out(0, pic2_data_port);
    }

    void send_end_of_interrupt(const uint8_t idx) {
        constexpr uint8_t end_of_interrupt = 0x20;

        if (idx >= 8) {
            PortIO::out(end_of_interrupt, pic2_command_port);
        }

        PortIO::out(end_of_interrupt, pic1_command_port);
    }

} // namespace Arch::I386::Interrupts::Pic8259
