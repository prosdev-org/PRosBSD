#include <console/video_console.hxx>

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
};

VideoConsoleOutputStream::VideoConsoleOutputStream(VideoConsole *video_console) {
    this->video_console = video_console;
    dimension_x = video_console->get_dimension_x();
    dimension_y = video_console->get_dimension_y();
    buffer = new VideoConsole::ColoredCharacter[dimension_x * dimension_y];
    idx = 0;
}

VideoConsoleOutputStream::~VideoConsoleOutputStream() {
    delete buffer;
}

void VideoConsoleOutputStream::write(const void *data, const size_t nbytes) {
    // TODO: control characters
    const auto chars = static_cast<const char *>(data);
    for (size_t i = 0; i < nbytes; i++) {
        if (idx > dimension_x * dimension_y) {
            return;
            // TODO: Scroll
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
            video_console->write(buffer[x + dimension_x * y], x, y);
        }
    }
}

OutputStream *VideoConsole::to_output_stream() {
    const auto output_stream = new VideoConsoleOutputStream(this);
    return output_stream;
}
