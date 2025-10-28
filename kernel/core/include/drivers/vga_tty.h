#ifndef VGA_TTY_H
#define VGA_TTY_H

#include <stdint.h>

void vga_tty_clear();
void vga_tty_putchar(char c);
void vga_tty_set_color(uint8_t fg, uint8_t bg);
void vga_tty_reset_color();

#endif
