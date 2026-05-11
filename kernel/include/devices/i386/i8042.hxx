#ifndef DEVICES_I386_I8042_HXX
#define DEVICES_I386_I8042_HXX

#include <devices/device.hxx>
#include <libkxx/linked_list.hxx>
#include <machine/interrupts.hxx>
#include <sys/keyboard.hxx>

namespace Devices::I386 {
    class I8042 final : public Device, public Machine::Interrupts::Handler, public Sys::Keyboard {
    public:
        static bool match(const AutoConf::MatchInfo &match_info);

        explicit I8042(
                const AutoConf::DriverHeader &driver_header,
                const AutoConf::MatchInfo &match_info);
        void handle() override;
        char get_next() override;
        bool has_next() override;

    private:
        static constexpr uint16_t data_port = 0x60;

        static constexpr uint16_t release_code_offset = 0x80;
        static constexpr char scancode_map_normal[255] = {
                '\0', '\0', '1', '2', '3', '4', '5', '6', // 00-07
                '7', '8', '9', '0', '-', '=', '\b', '\t', // 08-0F
                'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', // 10-17
                'o', 'p', '[', ']', '\n', '\0', 'a', 's', // 18-1F
                'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', // 20-27
                '\'', '`', '\0', '\\', 'z', 'x', 'c', 'v', // 28-2F
                'b', 'n', 'm', ',', '.', '/', '\0', '*', // 30-37
                '\0', ' ', '\0', '\0', '\0', '\0', '\0', '\0', // 38-3F
                '\0', '\0', '\0', '\0', '\0', '\0', '\0', '7', // 40-47
                '8', '9', '-', '4', '5', '6', '+', '1', // 48-4F
                '2', '3', '0', '.', '\0', '\0', '\0', '\0', // 50-57
                '\0'};

        static constexpr char scancode_map_shifted[255] = {
                '\0', '\0', '!', '@', '#', '$', '%', '^', // 00-07
                '&', '*', '(', ')', '_', '+', '\b', '\t', // 08-0F
                'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', // 10-17
                'O', 'P', '{', '}', '\n', '\0', 'A', 'S', // 18-1F
                'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', // 20-27
                '"', '~', '\0', '|', 'Z', 'X', 'C', 'V', // 28-2F
                'B', 'N', 'M', '<', '>', '?', '\0', '*', // 30-37
                '\0', ' ', '\0', '\0', '\0', '\0', '\0', '\0', // 38-3F
                '\0', '\0', '\0', '\0', '\0', '\0', '\0', '7', // 40-47
                '8', '9', '-', '4', '5', '6', '+', '1', // 48-4F
                '2', '3', '0', '.', '\0', '\0', '\0', '\0', // 50-57
                '\0'};

        static constexpr uint8_t scancode_left_shift = 0x2A;
        static constexpr uint8_t scancode_right_shift = 0x36;
        static constexpr uint8_t scancode_caps_lock = 0x3A;

        bool left_shift_pressed = false;
        bool right_shift_pressed = false;
        bool caps_locked = false;
        kxx::LinkedList<char> char_queue;
    };
} // namespace Devices::I386

#endif
