#ifndef CLGD64XX_H
#define CLGD64XX_H

#include <stdint.h>

typedef enum {
    CIRRUS_320x200x8 = 0x13,

    CIRRUS_640x480x8 = 0x5F,
    CIRRUS_640x480x15 = 0x66,
    CIRRUS_640x480x16 = 0x64,
    CIRRUS_640x480x24 = 0x71,

    CIRRUS_800x600x8 = 0x5C,
    CIRRUS_800x600x15 = 0x67,
    CIRRUS_800x600x16 = 0x65,
    CIRRUS_800x600x24 = 0x78,

    CIRRUS_1024x768x8 = 0x60,
    CIRRUS_1024x768x15 = 0x68,
    CIRRUS_1024x768x16 = 0x74,
    CIRRUS_1024x768x24 = 0x79,

    CIRRUS_1280x1024x8 = 0x6D,
    CIRRUS_1280x1024x15 = 0x69,
    CIRRUS_1280x1024x16 = 0x75,

    CIRRUS_1600x1200x8 = 0x7B,
} vga_mode_t;

#define VGA_FB_ADDR ((volatile uint8_t *) 0xA0000)

extern uint16_t vga_width;
extern uint16_t vga_height;
extern uint8_t vga_bpp;
extern uint32_t vga_stride;

void vga_init(vga_mode_t mode);

void vga_draw_pixel(uint16_t x, uint16_t y, uint32_t color);
void vga_clear(uint32_t color);

#endif // CLGD64XX_H
