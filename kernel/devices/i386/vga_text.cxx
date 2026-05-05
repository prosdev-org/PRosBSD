#include <devices/i386/isa_bus.hxx>
#include <devices/i386/vga_text.hxx>
#include <libkxx/string_view.hxx>
#include <string.h>
#include <string_v.h>
#include <sys/kernel.hxx>
#include <unique/assert.h>
#include <unique/countof.h>
#include <unique/log.hxx>

namespace Devices::I386 {
    static kxx::StringView get_logger_prefix() {
        return "devices/i386/vga_text";
    }

    void VgaText::ensure_init() {
        static bool initialized = false;
        if (initialized) {
            return;
        }

        ColorConverter::init();

        initialized = true;
    }

    bool VgaText::match(const AutoConf::MatchInfo &match_info) {
        ASSERT(match_info.parent != nullptr);

        if (kxx::StringView(match_info.parent->driver_header.name) != "isabus") {
            return false;
        }

        const IsaBus::ConnectionInfoConverter converter = {
                .general = match_info.bus_connection_info,
        };

        return converter.isa_bus->device_type == IsaBus::VgaText;
    }

    VgaText::VgaText(
            const AutoConf::DriverHeader &driver_header,
            const AutoConf::MatchInfo &match_info) : Device(driver_header, match_info.parent) {
        LOG("initializing");

        ensure_init();

        constexpr uintptr_t buffer_base = 0xC00B8000;
        this->buffer = reinterpret_cast<uint16_t *>(buffer_base);
        memset_v(buffer, 0, BUFFER_SIZE);

        Sys::Kernel::set_output_stream(as_output_stream());
    }

    void VgaText::write(const ColoredCharacter colored_character, const size_t x, const size_t y) {
        ASSERT(x < BUFFER_WIDTH);
        ASSERT(y < BUFFER_HEIGHT);

        buffer[x + BUFFER_WIDTH * y] = to_buf_el(colored_character);
    }

    size_t VgaText::get_dimension_x() {
        return BUFFER_WIDTH;
    }

    size_t VgaText::get_dimension_y() {
        return BUFFER_HEIGHT;
    }

    uint16_t VgaText::to_buf_el(const ColoredCharacter colored_character) {
        const uint8_t background = ColorConverter::convert(colored_character.background);
        const uint8_t foreground = ColorConverter::convert(colored_character.foreground);

        const uint8_t attribute = foreground | (background << 4);
        // static_cast<uint8_t> is required to avoid sign extension
        // e.g., -1 -> 255, not -1 -> 65535
        return static_cast<uint16_t>(static_cast<uint8_t>(colored_character.ch)) | (static_cast<uint16_t>(attribute) << 8);
    }

    uint8_t VgaText::ColorConverter::map[static_cast<size_t>(Color::_count)];

    void VgaText::ColorConverter::init() {
        memset(map, INVALID_COLOR, static_cast<size_t>(Color::_count));
        map[static_cast<size_t>(Color::Black)] = 0;
        map[static_cast<size_t>(Color::Blue)] = 1;
        map[static_cast<size_t>(Color::Green)] = 2;
        map[static_cast<size_t>(Color::Cyan)] = 3;
        map[static_cast<size_t>(Color::Red)] = 4;
        map[static_cast<size_t>(Color::Magenta)] = 5;
        map[static_cast<size_t>(Color::Brown)] = 6;
        map[static_cast<size_t>(Color::White)] = 7;
        map[static_cast<size_t>(Color::Gray)] = 8;
        map[static_cast<size_t>(Color::LightBlue)] = 9;
        map[static_cast<size_t>(Color::LightGreen)] = 10;
        map[static_cast<size_t>(Color::LightCyan)] = 11;
        map[static_cast<size_t>(Color::LightRed)] = 12;
        map[static_cast<size_t>(Color::LightMagenta)] = 13;
        map[static_cast<size_t>(Color::Yellow)] = 14;
        map[static_cast<size_t>(Color::BrightWhite)] = 15;
    }

    uint8_t VgaText::ColorConverter::convert(const Color color) {
        ASSERT(static_cast<size_t>(color) < COUNTOF(map));

        const uint8_t converted = map[static_cast<size_t>(color)];
        ASSERT(converted != INVALID_COLOR);

        return converted;
    }
} // namespace Devices::I386
