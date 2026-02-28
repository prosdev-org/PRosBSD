#ifndef CONSOLE_VIDEO_CONSOLE_H
#define CONSOLE_VIDEO_CONSOLE_H

#include <stddef.h>

typedef enum {
    VideoConsole_Character_Color_BLACK,
    VideoConsole_Character_Color_BLUE,
    VideoConsole_Character_Color_GREEN,
    VideoConsole_Character_Color_CYAN,
    VideoConsole_Character_Color_RED,
    VideoConsole_Character_Color_MAGENTA,
    VideoConsole_Character_Color_BROWN,
    VideoConsole_Character_Color_WHITE,
    VideoConsole_Character_Color_GRAY,
    VideoConsole_Character_Color_LIGHT_BLUE,
    VideoConsole_Character_Color_LIGHT_GREEN,
    VideoConsole_Character_Color_LIGHT_CYAN,
    VideoConsole_Character_Color_LIGHT_RED,
    VideoConsole_Character_Color_LIGHT_MAGENTA,
    VideoConsole_Character_Color_YELLOW,
    VideoConsole_Character_Color_BRIGHT_WHITE,
    VideoConsole_Character_Color_size
} VideoConsole_Character_Color;

typedef struct {
    char ch;
    VideoConsole_Character_Color foreground_color;
    VideoConsole_Character_Color background_color;
} VideoConsole_Character;

typedef struct {
    size_t (*get_dimension_x)();
    size_t (*get_dimension_y)();
    void (*write)(VideoConsole_Character character, size_t x, size_t y);
} VideoConsole;

#endif
