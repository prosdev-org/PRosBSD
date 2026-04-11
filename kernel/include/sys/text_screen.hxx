#ifndef SYS_TEXT_SCREEN_HXX
#define SYS_TEXT_SCREEN_HXX

#include <libkxx/unique_ptr.hxx>
#include <sys/output_stream.hxx>

namespace Sys {
    class TextScreen {
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

        virtual ~TextScreen() = default;
        virtual void write(ColoredCharacter colored_character, size_t x, size_t y) = 0;
        virtual size_t get_dimension_x() = 0;
        virtual size_t get_dimension_y() = 0;
        kxx::UniquePtr<OutputStream> as_output_stream();
    };
} // namespace Sys

#endif
