#include <arch/init.hxx>
#include <kernel.hxx>
#include <unique/extern_c.h>
#include <unique/noreturn.h>

EXTERN_C noreturn void Kernel_entry() {
    Arch::init();
}

namespace Kernel {
    // TODO: CharArrayOutputStream
    static VideoConsole video_console;

    void main() {
        // Temp demo
        for (size_t x = 0; x < video_console.get_dimension_x(); x++) {
            for (size_t y = 0; y < video_console.get_dimension_y(); y++) {
                video_console.write(
                        (VideoConsole_Character) {
                                '?', VideoConsole_Character_Color_BLACK,
                                VideoConsole_Character_Color_BRIGHT_WHITE},
                        x, y);
            }
        }

        for (;;)
            ;
    }

    void set_video_console(const VideoConsole new_video_console) {
        video_console = new_video_console;
    }
} // namespace Kernel
