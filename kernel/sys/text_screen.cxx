#include <libkxx/unique_ptr.hxx>
#include <string.h>
#include <sys/text_screen.hxx>

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
        kxx::UniquePtr<TextScreen::ColoredCharacter[]> buffer;
        size_t idx;

        void scroll_down();
    };

    TextScreenToOutputStreamAdapter::TextScreenToOutputStreamAdapter(TextScreen *text_screen) {
        this->text_screen = text_screen;
        dimension_x = text_screen->get_dimension_x();
        dimension_y = text_screen->get_dimension_y();
        buffer = kxx::UniquePtr<TextScreen::ColoredCharacter[]>(new TextScreen::ColoredCharacter[dimension_x * dimension_y]);
        memset(&buffer[0], 0, sizeof(buffer[0]) * dimension_x * dimension_y);
        idx = 0;
    }

    void TextScreenToOutputStreamAdapter::write(const void *data, const size_t nbytes) {
        // TODO: control characters
        const auto chars = static_cast<const char *>(data);
        for (size_t i = 0; i < nbytes; i++) {
            if (idx >= dimension_x * dimension_y) {
                scroll_down();
                idx = dimension_x * (dimension_y - 1);
            }

            if (chars[i] == '\n') {
                // y++
                idx -= idx % dimension_x;
                idx += dimension_x;
                continue;
            }

            buffer[idx] = {
                    chars[i],
                    TextScreen::Color::Black,
                    TextScreen::Color::BrightWhite},
            idx++;
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

    // NOLINTNEXTLINE(readability-make-member-function-const)
    void TextScreenToOutputStreamAdapter::scroll_down() {
        memmove(&buffer[0], &buffer[dimension_x], sizeof(buffer[0]) * dimension_x * (dimension_y - 1));
        memset(&buffer[dimension_x * (dimension_y - 1)], 0, sizeof(buffer[0]) * dimension_x);
    }

    kxx::UniquePtr<OutputStream> TextScreen::as_output_stream() {
        return kxx::UniquePtr(static_cast<OutputStream *>(new TextScreenToOutputStreamAdapter(this)));
    }
} // namespace Sys
