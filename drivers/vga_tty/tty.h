#ifndef TTY_H
#define TTY_H

#include <stdint.h>

#define VGA_BUFFER 0xB8000
#define COLS       80
#define ROWS       25
#define ATTRIBUTE  0x07

// Color attributes
#define VGA_BLACK         0x00
#define VGA_BLUE          0x01
#define VGA_GREEN         0x02
#define VGA_CYAN          0x03
#define VGA_RED           0x04
#define VGA_MAGENTA       0x05
#define VGA_BROWN         0x06
#define VGA_LIGHT_GREY    0x07
#define VGA_DARK_GREY     0x08
#define VGA_LIGHT_BLUE    0x09
#define VGA_LIGHT_GREEN   0x0A
#define VGA_LIGHT_CYAN    0x0B
#define VGA_LIGHT_RED     0x0C
#define VGA_LIGHT_MAGENTA 0x0D
#define VGA_LIGHT_BROWN   0x0E
#define VGA_WHITE         0x0F

// Global variables
extern uint8_t current_color;

// Function declarations
void putck_colored(char c, uint8_t color);
void putsk_colored(const char *str, uint8_t color);
void set_current_color(uint8_t color);
uint8_t get_current_color(void);

#endif // TTY_H
