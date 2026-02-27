#include <i386/drivers/vga_text.h>
#include <stdint.h>
#include <string_v.h>

#define BUFFER_WIDTH  80
#define BUFFER_HEIGHT 25
#define BUFFER_COUNT  (BUFFER_WIDTH * BUFFER_HEIGHT)
#define BUFFER_BASE   0xC00B8000

#define INVALID_COLOR 0xFF

// TODO: Memory mapped buffer
static volatile uint16_t *buffer = (uint16_t *) BUFFER_BASE;

static uint8_t color_convertion_map[VideoConsole_Character_Color_size];

static void color_convertion_map_init() {
    memset(color_convertion_map, INVALID_COLOR, VideoConsole_Character_Color_size);
    color_convertion_map[VideoConsole_Character_Color_BLACK] = 0;
    color_convertion_map[VideoConsole_Character_Color_BLUE] = 1;
    color_convertion_map[VideoConsole_Character_Color_GREEN] = 2;
    color_convertion_map[VideoConsole_Character_Color_CYAN] = 3;
    color_convertion_map[VideoConsole_Character_Color_RED] = 4;
    color_convertion_map[VideoConsole_Character_Color_MAGENTA] = 5;
    color_convertion_map[VideoConsole_Character_Color_BROWN] = 6;
    color_convertion_map[VideoConsole_Character_Color_WHITE] = 7;
    color_convertion_map[VideoConsole_Character_Color_GRAY] = 8;
    color_convertion_map[VideoConsole_Character_Color_LIGHT_BLUE] = 9;
    color_convertion_map[VideoConsole_Character_Color_LIGHT_GREEN] = 10;
    color_convertion_map[VideoConsole_Character_Color_LIGHT_CYAN] = 11;
    color_convertion_map[VideoConsole_Character_Color_LIGHT_RED] = 12;
    color_convertion_map[VideoConsole_Character_Color_LIGHT_MAGENTA] = 13;
    color_convertion_map[VideoConsole_Character_Color_YELLOW] = 14;
    color_convertion_map[VideoConsole_Character_Color_BRIGHT_WHITE] = 15;
}

static uint8_t convert_color(const VideoConsole_Character_Color color) {
    const uint8_t converted = color_convertion_map[color];
    // if (converted == INVALID_COLOR) {
    //     // TODO: panic
    // }

    return converted;
}

static uint16_t character_to_el(const VideoConsole_Character character) {
    const uint8_t foreground_color = convert_color(character.foreground_color);
    const uint8_t background_color = convert_color(character.background_color);

    const uint8_t attribute = foreground_color | (background_color << 4);
    return (uint16_t) character.ch | ((uint16_t) attribute << 8);
}

static size_t get_dimension_x() {
    return BUFFER_WIDTH;
}

static size_t get_dimension_y() {
    return BUFFER_HEIGHT;
}

static void write(const VideoConsole_Character character, const size_t x, const size_t y) {
    if (x >= get_dimension_x() || y >= get_dimension_y()) {
        // TODO: panic
        return;
    }

    buffer[x + BUFFER_WIDTH * y] = character_to_el(character);
}

VideoConsole i386_Drivers_VgaText_init() {
    color_convertion_map_init();

    memset_v(buffer, 0, sizeof(buffer[0]) * BUFFER_COUNT);

    const VideoConsole video_console = {get_dimension_x, get_dimension_y, write};
    return video_console;
}
