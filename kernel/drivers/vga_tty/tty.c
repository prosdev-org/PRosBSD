#include <core/panic.h>
#include <drivers/tty.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define VGA_BUFFER        0xC00B8000
#define COLS              80
#define ROWS              25
#define DEFAULT_ATTRIBUTE 0x07

// VGA attributes (internal use only)
#define VGA_ATTR_BLINK     0x80
#define VGA_ATTR_BRIGHT    0x08
#define VGA_ATTR_UNDERLINE 0x01

// VGA colors (internal use only)
#define VGA_BLACK         0
#define VGA_BLUE          1
#define VGA_GREEN         2
#define VGA_CYAN          3
#define VGA_RED           4
#define VGA_MAGENTA       5
#define VGA_BROWN         6
#define VGA_LIGHT_GREY    7
#define VGA_DARK_GREY     8
#define VGA_LIGHT_BLUE    9
#define VGA_LIGHT_GREEN   10
#define VGA_LIGHT_CYAN    11
#define VGA_LIGHT_RED     12
#define VGA_LIGHT_MAGENTA 13
#define VGA_YELLOW        14
#define VGA_WHITE         15

uint32_t cursor_row = 0;
uint32_t cursor_col = 0;
uint8_t current_attribute = DEFAULT_ATTRIBUTE;
bool in_escape = false;
bool in_csi = false;
char escape_buffer[32];
int escape_pos = 0;

// VGA text mode attribute byte:
// 7    6 5 4  3    2 1 0
// Blink BG   Bright FG
//
// Bit 3 is used for bright foreground colors
// Bit 7 is used for blinking text
// Bit 1 is used for underlining in monochrome mode

void set_color(uint8_t fg, uint8_t bg) {
    // Preserve special attributes (blink, etc.)
    uint8_t special = current_attribute & 0x80; // Keep only the blink bit
    current_attribute = special | (bg << 4) | (fg & 0x0F);
}

void reset_color() {
    current_attribute = DEFAULT_ATTRIBUTE;
}

void cleark() {
    volatile uint16_t *vga = (volatile uint16_t *) VGA_BUFFER;
    for (uint32_t i = 0; i < COLS * ROWS; i++) {
        vga[i] = (current_attribute << 8) | ' ';
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
        vga[last_row + col] = (current_attribute << 8) | ' ';
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
    vga[idx] = (current_attribute << 8) | ' ';
}

static void clear_screen() {
    for (uint32_t i = 0; i < COLS * ROWS; i++) {
        volatile uint16_t *vga = (volatile uint16_t *) VGA_BUFFER;
        vga[i] = (current_attribute << 8) | ' ';
    }
    cursor_row = 0;
    cursor_col = 0;
}

static void clear_line_from_cursor() {
    volatile uint16_t *vga = (volatile uint16_t *) VGA_BUFFER;
    uint32_t start = cursor_row * COLS + cursor_col;
    uint32_t end = (cursor_row + 1) * COLS;

    for (uint32_t i = start; i < end; i++) {
        vga[i] = (current_attribute << 8) | ' ';
    }
}

static void move_cursor(int n, char direction) {
    switch (direction) {
        case 'A': // Up
            if (cursor_row >= n) {
                cursor_row -= n;
            } else {
                cursor_row = 0;
            }
            break;
        case 'B': // Down
            cursor_row += n;
            if (cursor_row >= ROWS) {
                cursor_row = ROWS - 1;
            }
            break;
        case 'C': // Forward
            cursor_col += n;
            if (cursor_col >= COLS) {
                cursor_col = COLS - 1;
            }
            break;
        case 'D': // Back
            if (cursor_col >= n) {
                cursor_col -= n;
            } else {
                cursor_col = 0;
            }
            break;
    }
}

static void handle_color_param(int param) {
    uint8_t fg = current_attribute & 0x0F;
    uint8_t bg = (current_attribute >> 4) & 0x0F;

    switch (param) {
        case 0: // Reset
            reset_color();
            break;
        case 1: // Bright/Bold
            // Use high-intensity VGA colors (8-15) for bright colors
            if (fg < 8)
                fg += 8;
            set_color(fg, bg);
            break;
        case 4: // Underline
            // Emulate underlining with blue background in color mode
            bg = VGA_BLUE;
            set_color(fg, bg);
            break;
        case 5: // Blink
            current_attribute |= 0x80; // Set blink bit
            break;
        case 7: // Inverse
            set_color(bg, fg);
            break;
        case 30: // Black foreground
            set_color(VGA_BLACK, bg);
            break;
        case 31: // Red foreground
            set_color(VGA_RED, bg);
            break;
        case 32: // Green foreground
            set_color(VGA_GREEN, bg);
            break;
        case 33: // Yellow foreground
            set_color(VGA_BROWN, bg);
            break;
        case 34: // Blue foreground
            set_color(VGA_BLUE, bg);
            break;
        case 35: // Magenta foreground
            set_color(VGA_MAGENTA, bg);
            break;
        case 36: // Cyan foreground
            set_color(VGA_CYAN, bg);
            break;
        case 37: // White foreground
            set_color(VGA_LIGHT_GREY, bg);
            break;
        case 40: // Black background
            set_color(fg, VGA_BLACK);
            break;
        case 41: // Red background
            set_color(fg, VGA_RED);
            break;
        case 42: // Green background
            set_color(fg, VGA_GREEN);
            break;
        case 43: // Yellow background
            set_color(fg, VGA_BROWN);
            break;
        case 44: // Blue background
            set_color(fg, VGA_BLUE);
            break;
        case 45: // Magenta background
            set_color(fg, VGA_MAGENTA);
            break;
        case 46: // Cyan background
            set_color(fg, VGA_CYAN);
            break;
        case 47: // White background
            set_color(fg, VGA_LIGHT_GREY);
            break;
        // Bright foreground colors
        case 90: // Bright black (gray) foreground
            set_color(VGA_DARK_GREY, bg);
            break;
        case 91: // Bright red foreground
            set_color(VGA_LIGHT_RED, bg);
            break;
        case 92: // Bright green foreground
            set_color(VGA_LIGHT_GREEN, bg);
            break;
        case 93: // Bright yellow foreground
            set_color(VGA_YELLOW, bg);
            break;
        case 94: // Bright blue foreground
            set_color(VGA_LIGHT_BLUE, bg);
            break;
        case 95: // Bright magenta foreground
            set_color(VGA_LIGHT_MAGENTA, bg);
            break;
        case 96: // Bright cyan foreground
            set_color(VGA_LIGHT_CYAN, bg);
            break;
        case 97: // Bright white foreground
            set_color(VGA_WHITE, bg);
            break;
    }
}

static int parse_decimal(int *pos) {
    int result = 0;
    while (*pos < escape_pos - 1 && escape_buffer[*pos] >= '0' && escape_buffer[*pos] <= '9') {
        result = result * 10 + (escape_buffer[*pos] - '0');
        (*pos)++;
    }
    return result;
}

void handle_csi_sequence() {
    escape_buffer[escape_pos] = '\0';
    char terminator = escape_buffer[escape_pos - 1];
    int pos = 0;

    switch (terminator) {
        case 'm': // SGR - Select Graphic Rendition
            // If string is empty or contains only 'm', use parameter 0
            if (escape_pos <= 1) {
                handle_color_param(0);
                break;
            }

            // Process all parameters separated by semicolons
            while (pos < escape_pos - 1) {
                int param = parse_decimal(&pos);
                handle_color_param(param);

                // Skip semicolon
                if (escape_buffer[pos] == ';') {
                    pos++;
                }
            }
            break;

        case 'H': // Cursor Position
        case 'f': // Force Cursor Position
            cursor_row = 0;
            cursor_col = 0;
            break;

        case 'J': // Clear Screen
            pos = 0;
            int mode = (escape_pos > 1) ? parse_decimal(&pos) : 0;
            if (mode == 2) {
                clear_screen();
            }
            break;

        case 'K': // Clear Line
            clear_line_from_cursor();
            break;

        case 'A': // Cursor Up
        case 'B': // Cursor Down
        case 'C': // Cursor Forward
        case 'D': // Cursor Back
            pos = 0;
            int n = (escape_pos > 1) ? parse_decimal(&pos) : 1;
            move_cursor(n, terminator);
            break;
    }

    // Reset escape sequence state
    in_escape = false;
    in_csi = false;
    escape_pos = 0;
}

void putck(char c) {
    volatile uint16_t *vga = (volatile uint16_t *) VGA_BUFFER;

    if (in_escape) {
        if (in_csi) {
            if (escape_pos < sizeof(escape_buffer) - 1) {
                escape_buffer[escape_pos++] = c;
            }
            if (c >= 64 && c <= 126) { // End of CSI sequence
                handle_csi_sequence();
            }
            return;
        } else if (c == '[') {
            in_csi = true;
            return;
        } else {
            in_escape = false;
        }
    } else if (c == '\033') { // ESC
        in_escape = true;
        escape_pos = 0;
        return;
    }

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
                vga[idx] = (current_attribute << 8) | c;
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
