#include "tty.h"
#include <extrns.h>
#include <stdint.h>
#include <tty.h>

uint32_t cursor_row = 0;
uint32_t cursor_col = 0;

void cleark() {
    volatile uint16_t *vga = (volatile uint16_t *) VGA_BUFFER;
    for (uint32_t i = 0; i < COLS * ROWS; i++) {
        vga[i] = (ATTRIBUTE << 8) | ' ';
    }
    cursor_row = 0;
    cursor_col = 0;
}

void scroll() {
    volatile uint16_t *vga = (volatile uint16_t *) VGA_BUFFER;

    // shift lines 1-24 up to lines 0-23
    for (uint32_t row = 0; row < ROWS - 1; row++) {
        for (uint32_t col = 0; col < COLS; col++) {
            uint32_t dst_idx = row * COLS + col;
            uint32_t src_idx = (row + 1) * COLS + col;
            vga[dst_idx] = vga[src_idx];
        }
    }

    // clear the last line (row 24)
    uint32_t last_row = (ROWS - 1) * COLS;
    for (uint32_t col = 0; col < COLS; col++) {
        vga[last_row + col] = (ATTRIBUTE << 8) | ' ';
    }

    cursor_row = ROWS - 1; // move cursor to start of last line
    cursor_col = 0;
}

void handle_backspace() {
    volatile uint16_t *vga = (volatile uint16_t *) VGA_BUFFER;

    if (cursor_row == 0 && cursor_col == 0) {
        return; // already at top-left, nothing to delete
    }

    if (cursor_col > 0) {
        cursor_col--;
    } else {
        cursor_row--;
        cursor_col = COLS - 1;
    }

    // overwrite character with space
    uint32_t idx = cursor_row * COLS + cursor_col;
    vga[idx] = (ATTRIBUTE << 8) | ' ';
}

void putck(char c) {
    volatile uint16_t *vga = (volatile uint16_t *) VGA_BUFFER;

    switch (c) {
        case '\n': // newline: move to next line
            cursor_col = 0;
            cursor_row++;
            break;
        case '\r': // carriage return: return to start of line
            cursor_col = 0;
            break;
        case '\t': // tab: advance to next tab stop (every 4 spaces)
            cursor_col = (cursor_col + 4) & ~3;
            break;
        case '\b': // backspace
            handle_backspace();
            break;
        default: // printable character
            if (c >= ' ') {
                uint32_t idx = cursor_row * COLS + cursor_col;
                if (idx >= COLS * ROWS) {
                    panic("tty: buffer overflow");
                }
                vga[idx] = (ATTRIBUTE << 8) | c;
                cursor_col++;
            }
            break;
    }

    // handle line wrap
    if (cursor_col >= COLS) {
        cursor_col = 0;
        cursor_row++;
    }

    // scroll if at the bottom
    if (cursor_row >= ROWS) {
        scroll();
    }
}

void putsk(const char *str) {
    while (*str) {
        putck(*str++);
    }
    putck('\n');
}
