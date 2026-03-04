#include <arch/init.hxx>
#include <kernel.hxx>
#include <unique/extern_c.h>
#include <unique/noreturn.h>

EXTERN_C noreturn void Kernel_entry() {
    Arch::init();
}

namespace Kernel {
    // TODO: CharArrayOutputStream
    static VideoConsole *video_console;

    void main() {
        // Temp demo
        const size_t dimension_x = video_console->get_dimension_x();
        const size_t dimension_y = video_console->get_dimension_y();
        for (size_t x = 0; x < dimension_x; x++) {
            for (size_t y = 0; y < dimension_y; y++) {
                video_console->write(
                        {'a',
                         VideoConsole::Color::Black,
                         VideoConsole::Color::White},
                        x, y);
            }
        }

        for (;;)
            ;
    }

    void set_video_console(VideoConsole *new_video_console) {
        video_console = new_video_console;
    }
} // namespace Kernel
