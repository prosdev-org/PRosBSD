#include <string.h>
#include <sys/console/video_console.hxx>

namespace Sys {
    class VideoConsoleOutputStream final : public OutputStream {
    public:
        explicit VideoConsoleOutputStream(VideoConsole *video_console);
        ~VideoConsoleOutputStream() override;
        void write(const void *data, size_t nbytes) override;
        void flush() override;

    private:
        VideoConsole *video_console;
        size_t dimension_x;
        size_t dimension_y;
        VideoConsole::ColoredCharacter *buffer;
        size_t idx;

        void scroll_down();
    };

    VideoConsoleOutputStream::VideoConsoleOutputStream(VideoConsole *video_console) {
        this->video_console = video_console;
        dimension_x = video_console->get_dimension_x();
        dimension_y = video_console->get_dimension_y();
        buffer = new VideoConsole::ColoredCharacter[dimension_x * dimension_y];
        memset(buffer, 0, sizeof(buffer[0]) * dimension_x * dimension_y);
        idx = 0;
    }

    VideoConsoleOutputStream::~VideoConsoleOutputStream() {
        delete buffer;
    }

    void VideoConsoleOutputStream::write(const void *data, const size_t nbytes) {
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
                    VideoConsole::Color::Black,
                    VideoConsole::Color::BrightWhite},
            idx++;
        }
    }

    void VideoConsoleOutputStream::flush() {
        for (size_t y = 0; y < dimension_y; y++) {
            for (size_t x = 0; x < dimension_x; x++) {
                video_console->write(
                        kxx::Ref<const VideoConsole::ColoredCharacter>(buffer[x + dimension_x * y]),
                        x, y);
            }
        }
    }

    // NOLINTNEXTLINE(readability-make-member-function-const)
    void VideoConsoleOutputStream::scroll_down() {
        memmove(buffer, (buffer + dimension_x), sizeof(buffer[0]) * dimension_x * (dimension_y - 1));
        memset(buffer + dimension_x * (dimension_y - 1), 0, sizeof(buffer[0]) * dimension_x);
    }

    OutputStream *VideoConsole::to_output_stream() {
        const auto output_stream = new VideoConsoleOutputStream(this);
        return output_stream;
    }
} // namespace Sys
