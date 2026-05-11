#include <arch/i386/port_io.hxx>
#include <devices/i386/i8042.hxx>
#include <devices/i386/isa_bus.hxx>
#include <libkxx/string_view.hxx>
#include <sys/kernel.hxx>
#include <unique/countof.h>
#include <unique/log.hxx>

namespace Devices::I386 {
    static kxx::StringView get_logger_prefix() {
        return "devices/i386/i8042";
    }

    bool I8042::match(const AutoConf::MatchInfo &match_info) {
        ASSERT(match_info.parent != nullptr);

        if (kxx::StringView(match_info.parent->driver_header.name) != "isabus") {
            return false;
        }

        const IsaBus::ConnectionInfoConverter converter = {
                .general = match_info.bus_connection_info,
        };

        return converter.isa_bus->device_type == IsaBus::I8042;
    }

    I8042::I8042(
            const AutoConf::DriverHeader &driver_header,
            const AutoConf::MatchInfo &match_info) : Device(driver_header, match_info.parent) {
        LOG("initializing");
        // TODO: init I8042

        constexpr uint8_t interrupt_idx = 1;
        Machine::Interrupts::bind_handler(this, interrupt_idx);

        Sys::Kernel::set_input_stream(as_input_stream());
    }

    void I8042::handle() {
        const uint8_t scancode = Arch::I386::PortIO::in(data_port);

        if ((scancode & release_code_offset) != 0) {
            switch (scancode & (~release_code_offset)) {
                case scancode_left_shift: {
                    left_shift_pressed = false;
                } break;
                case scancode_right_shift: {
                    right_shift_pressed = false;
                } break;
                default:;
            }

            return;
        }

        switch (scancode) {
            case scancode_left_shift: {
                left_shift_pressed = true;
                return;
            };
            case scancode_right_shift: {
                right_shift_pressed = true;
                return;
            };
            case scancode_caps_lock: {
                caps_locked = !caps_locked;
                return;
            };
            default:;
        }

        if ((left_shift_pressed | right_shift_pressed) ^ caps_locked) {
            ASSERT(scancode < COUNTOF(scancode_map_shifted));
            char_queue.insert_back(scancode_map_shifted[scancode]);
        } else {
            ASSERT(scancode < COUNTOF(scancode_map_normal));
            char_queue.insert_back(scancode_map_normal[scancode]);
        }
    }

    char I8042::get_next() {
        const char result = char_queue.get(0);
        char_queue.remove(0);
        return result;
    }

    bool I8042::has_next() {
        return !char_queue.is_empty();
    }
} // namespace Devices::I386
