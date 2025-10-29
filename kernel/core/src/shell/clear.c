#include <drivers/vga_tty.h>
#include <shell/clear.h>
#include <stdio.h>

int shell_clear(const int argc, const char **argv) {
    if (argc != 1) {
        printf("Usage: clear\n");
        return 1;
    }
    vga_tty_clear();
    return 0;
}
