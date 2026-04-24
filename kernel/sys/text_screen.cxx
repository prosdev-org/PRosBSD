#include <ctype.h>
#include <libkxx/unique_ptr.hxx>
#include <string.h>
#include <sys/text_screen.hxx>
#include <unique/assert.h>

namespace Sys {
    class TextScreenToOutputStreamAdapter final : public OutputStream {
    public:
        explicit TextScreenToOutputStreamAdapter(TextScreen *text_screen);
        void write(const void *data, size_t nbytes) override;
        void flush() override;

    private:
        TextScreen *text_screen;
        size_t dimension_x;
        size_t dimension_y;
        TextScreen::Color background;
        TextScreen::Color foreground;
        kxx::UniquePtr<TextScreen::ColoredCharacter[]> buffer;
        size_t idx;

        void handle_cntrl(char ch);
        void handle_printable(char ch);
        void ensure_scroll();
        void scroll();
        void write_char(char ch, size_t i);
    };

    kxx::UniquePtr<OutputStream> TextScreen::as_output_stream() {
        return kxx::UniquePtr(static_cast<OutputStream *>(new TextScreenToOutputStreamAdapter(this)));
    }

    TextScreenToOutputStreamAdapter::TextScreenToOutputStreamAdapter(TextScreen *text_screen) {
        this->text_screen = text_screen;
        dimension_x = text_screen->get_dimension_x();
        dimension_y = text_screen->get_dimension_y();

        background = TextScreen::Color::Black;
        foreground = TextScreen::Color::BrightWhite;

        buffer = kxx::UniquePtr<TextScreen::ColoredCharacter[]>(new TextScreen::ColoredCharacter[dimension_x * dimension_y]);
        for (size_t i = 0; i < dimension_x * dimension_y; i++) {
            write_char('\0', i);
        }

        idx = 0;
    }

    void TextScreenToOutputStreamAdapter::write(const void *data, const size_t nbytes) {
        ASSERT(data != nullptr);

        const auto chars = static_cast<const char *>(data);
        for (size_t i = 0; i < nbytes; i++) {
            ensure_scroll();

            if (iscntrl(chars[i])) {
                handle_cntrl(chars[i]);
            } else {
                handle_printable(chars[i]);
            }

            ensure_scroll();
        }
    }

    void TextScreenToOutputStreamAdapter::flush() {
        for (size_t y = 0; y < dimension_y; y++) {
            for (size_t x = 0; x < dimension_x; x++) {
                text_screen->write(
                        buffer[x + dimension_x * y],
                        x, y);
            }
        }
    }

    void TextScreenToOutputStreamAdapter::handle_cntrl(const char ch) {
        switch (ch) {
            case '\n': {
                const size_t new_idx = (idx / dimension_x + 1) * dimension_x;
                for (size_t i = idx; i < new_idx; i++) {
                    write_char('\0', i);
                }
                idx = new_idx;
            } break;
            case '\b': {
                if (idx != 0) {
                    idx--;
                }
                write_char('\0', idx);
            } break;
            case '\t': {
                constexpr size_t tab_width = 4;
                idx = (idx / tab_width + 1) * tab_width;
            } break;
            default:;
        }
    }

    void TextScreenToOutputStreamAdapter::handle_printable(const char ch) {
        write_char(ch, idx);
        idx++;
    }

    void TextScreenToOutputStreamAdapter::ensure_scroll() {
        while (idx >= dimension_x * dimension_y) {
            scroll();
            idx = dimension_x * (dimension_y - 1);
        }
    }

    // NOLINTNEXTLINE(readability-make-member-function-const)
    void TextScreenToOutputStreamAdapter::scroll() {
        memmove(&buffer[0], &buffer[dimension_x], sizeof(buffer[0]) * dimension_x * (dimension_y - 1));
        for (size_t i = dimension_x * (dimension_y - 1); i < dimension_x * dimension_y; i++) {
            write_char('\0', i);
        }
    }

    // NOLINTNEXTLINE(readability-make-member-function-const)
    void TextScreenToOutputStreamAdapter::write_char(const char ch, const size_t i) {
        buffer[i] = {
                ch,
                background,
                foreground};
    }
} // namespace Sys
