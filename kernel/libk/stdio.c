#include <drivers/keyboard.h>
#include <drivers/tty.h>
#include <stdio.h>

int getchar() {
    return keyboard_getchar();
}

int putchar(const int c) {
    vga_tty_putchar((char) c);
    return (char) c;
}

int puts(const char *str) {
    while (*str) {
        putchar(*str++);
    }
    putchar('\n');
    return (char) str;
}
