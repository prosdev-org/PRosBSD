#include <drivers/i386/vga_text.hxx>
#include <string.h>
#include <string_v.h>
#include <sys/panic.hxx>

bool Drivers::I386::VgaText::initialized = false;

void Drivers::I386::VgaText::init() {
    if (initialized) {
        return;
    }

    ColorConverter::init();

    initialized = true;
}

Drivers::I386::VgaText::VgaText(const uintptr_t buffer_base) {
    init();

    this->buffer = reinterpret_cast<uint16_t *>(buffer_base);

    memset_v(buffer, 0, BUFFER_SIZE);
}

void Drivers::I386::VgaText::write(const ColoredCharacter &colored_character, const size_t x, const size_t y) {
    if (x >= BUFFER_WIDTH || y >= BUFFER_HEIGHT) {
        Sys::panic("VGA Text: Invalid coordinates");
    }

    buffer[x + BUFFER_WIDTH * y] = to_buf_el(colored_character);
}

size_t Drivers::I386::VgaText::get_dimension_x() {
    return BUFFER_WIDTH;
}

size_t Drivers::I386::VgaText::get_dimension_y() {
    return BUFFER_HEIGHT;
}

uint16_t Drivers::I386::VgaText::to_buf_el(const ColoredCharacter &colored_character) {
    const uint8_t background = ColorConverter::convert(colored_character.background);
    const uint8_t foreground = ColorConverter::convert(colored_character.foreground);

    const uint8_t attribute = foreground | (background << 4);
    // static_cast<uint8_t> is required to avoid sign extension
    // e.g., -1 -> 255, not -1 -> 65535
    return static_cast<uint16_t>(static_cast<uint8_t>(colored_character.ch)) | (static_cast<uint16_t>(attribute) << 8);
}

uint8_t Drivers::I386::VgaText::ColorConverter::map[static_cast<size_t>(Color::_count)];

void Drivers::I386::VgaText::ColorConverter::init() {
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

uint8_t Drivers::I386::VgaText::ColorConverter::convert(const Color color) {
    const uint8_t converted = map[static_cast<size_t>(color)];
    if (converted == INVALID_COLOR) {
        Sys::panic("VGA Text: Invalid color");
    }
    return converted;
}
