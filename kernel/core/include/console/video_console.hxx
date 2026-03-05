#ifndef CONSOLE_VIDEO_CONSOLE_HXX
#define CONSOLE_VIDEO_CONSOLE_HXX

#include <stddef.h>

class VideoConsole {
public:
    enum class Color {
        Black,
        Blue,
        Green,
        Cyan,
        Red,
        Magenta,
        Brown,
        White,
        Gray,
        LightBlue,
        LightGreen,
        LightCyan,
        LightRed,
        LightMagenta,
        Yellow,
        BrightWhite,
        _count
    };

    struct ColoredCharacter {
        char ch;
        Color background;
        Color foreground;
    };

    virtual ~VideoConsole() = default;
    virtual void write(const ColoredCharacter &colored_character, size_t x, size_t y) = 0;
    virtual size_t get_dimension_x() = 0;
    virtual size_t get_dimension_y() = 0;
    // TODO: flush()
};

#endif
