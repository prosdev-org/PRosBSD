#include <drivers/keyboard.h>
#include <drivers/tty.h>
#include <stdio.h>

int getchar() {
    const int c = (unsigned char) keyboard_getchar();
    putchar(c);
    return c;
}

int putchar(const int c) {
    vga_tty_putchar((char) c);
    return (char) c;
}

char *gets_s(char *str, const size_t size) {
    char *s = str;
    int c;
    while ((size_t) (s - str) < (size - 1) && (c = getchar()) != '\n') {
        if (c == '\b') {
            if (s != str) {
                s--;
            }
            continue;
        }
        *s++ = (char) c;
    }
    *s = '\0';
    return str;
}

int puts(const char *str) {
    while (*str) {
        putchar(*str++);
    }
    putchar('\n');
    return (int) str;
}
