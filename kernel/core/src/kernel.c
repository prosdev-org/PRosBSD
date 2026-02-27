#include <console/video_console.h>
#include <i386/drivers/vga_text.h>
#include <unique/noreturn.h>

// TODO: CharArrayOutputStream
static VideoConsole video_console;

noreturn void Kernel_entry() {
    // TODO: Move to arch init
    video_console = i386_Drivers_VgaText_init();

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
