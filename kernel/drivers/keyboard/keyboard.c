#include <core/panic.h>
#include <drivers/keyboard.h>
#include <stdint.h>
#include <util/io.h>

#define KEYBOARD_DATA_PORT    0x60
#define KEYBOARD_STATUS_PORT  0x64
#define OUTPUT_BUFFER_FULL    0x01
#define RELEASE_CODE_OFFSET   0x80
#define LEFT_SHIFT            0x2A
#define RIGHT_SHIFT           0x36
#define CAPS_LOCK             0x3A
#define NUM_LOCK              0x45
#define SCROLL_LOCK           0x46
#define BACKSPACE             0x0E
#define ENTER                 0x1C
#define INPUT_BUFFER_FULL     0x02
#define KEYBOARD_CMD_SET_LEDS 0xED
#define LED_SCROLL_LOCK       0x01
#define LED_NUM_LOCK          0x02
#define LED_CAPS_LOCK         0x04

uint8_t shift_active = 0;
uint8_t caps_lock_active = 0;
uint8_t num_lock_active = 0;
uint8_t scroll_lock_active = 0;

// US QWERTY scancode maps
const char scancode_map_normal[255] = {0,    0,   '1', '2',  '3',  '4', '5',  '6', // 00-07
                                       '7',  '8', '9', '0',  '-',  '=', '\b', '\t', // 08-0F
                                       'q',  'w', 'e', 'r',  't',  'y', 'u',  'i', // 10-17
                                       'o',  'p', '[', ']',  '\n', 0,   'a',  's', // 18-1F
                                       'd',  'f', 'g', 'h',  'j',  'k', 'l',  ';', // 20-27
                                       '\'', '`', 0,   '\\', 'z',  'x', 'c',  'v', // 28-2F
                                       'b',  'n', 'm', ',',  '.',  '/', 0,    '*', // 30-37
                                       0,    ' ', 0,   0,    0,    0,   0,    0, // 38-3F
                                       0,    0,   0,   0,    0,    0,   0,    '7', // 40-47
                                       '8',  '9', '-', '4',  '5',  '6', '+',  '1', // 48-4F
                                       '2',  '3', '0', '.',  0,    0,   0,    0, // 50-57
                                       0};

const char scancode_map_shifted[255] = {0,   0,   '!', '@', '#',  '$', '%',  '^', // 00-07
                                        '&', '*', '(', ')', '_',  '+', '\b', '\t', // 08-0F
                                        'Q', 'W', 'E', 'R', 'T',  'Y', 'U',  'I', // 10-17
                                        'O', 'P', '{', '}', '\n', 0,   'A',  'S', // 18-1F
                                        'D', 'F', 'G', 'H', 'J',  'K', 'L',  ':', // 20-27
                                        '"', '~', 0,   '|', 'Z',  'X', 'C',  'V', // 28-2F
                                        'B', 'N', 'M', '<', '>',  '?', 0,    '*', // 30-37
                                        0,   ' ', 0,   0,   0,    0,   0,    0, // 38-3F
                                        0,   0,   0,   0,   0,    0,   0,    '7', // 40-47
                                        '8', '9', '-', '4', '5',  '6', '+',  '1', // 48-4F
                                        '2', '3', '0', '.', 0,    0,   0,    0, // 50-57
                                        0};

void set_keyboard_leds() {
    uint8_t led_status = 0;
    led_status |= scroll_lock_active ? LED_SCROLL_LOCK : 0;
    led_status |= num_lock_active ? LED_NUM_LOCK : 0;
    led_status |= caps_lock_active ? LED_CAPS_LOCK : 0;

    while (inb(KEYBOARD_STATUS_PORT) & INPUT_BUFFER_FULL)
        ;

    outb(KEYBOARD_CMD_SET_LEDS, KEYBOARD_DATA_PORT);

    while (!(inb(KEYBOARD_STATUS_PORT) & OUTPUT_BUFFER_FULL))
        ;
    const uint8_t ack = inb(KEYBOARD_DATA_PORT);
    if (ack != 0xFA)
        return;

    // wait for free
    while (inb(KEYBOARD_STATUS_PORT) & INPUT_BUFFER_FULL)
        ;

    outb(led_status, KEYBOARD_DATA_PORT);

    while (!(inb(KEYBOARD_STATUS_PORT) & OUTPUT_BUFFER_FULL))
        ;
    inb(KEYBOARD_DATA_PORT);
}

void keyboard_init() {
    // wait for input buffer to be clear
    while (inb(KEYBOARD_STATUS_PORT) & 0x02)
        ;

    // enable keyboard
    outb(0xAE, KEYBOARD_STATUS_PORT);

    set_keyboard_leds();
}

uint8_t keyboard_status() {
    return inb(KEYBOARD_STATUS_PORT);
}

uint8_t read_scancode() {
    while (!(keyboard_status() & OUTPUT_BUFFER_FULL))
        ;
    return inb(KEYBOARD_DATA_PORT);
}

// convert scancode to ASCII character
char scancode_to_ascii(const uint8_t scancode) {
    // handle key releases
    if (scancode & RELEASE_CODE_OFFSET) {
        const uint8_t released_key = scancode & ~RELEASE_CODE_OFFSET;

        if (released_key == LEFT_SHIFT || released_key == RIGHT_SHIFT) {
            shift_active = 0;
        }
        return 0;
    }

    // handle special keys
    switch (scancode) {
        case LEFT_SHIFT:
        case RIGHT_SHIFT:
            shift_active = 1;
            return 0;
        case CAPS_LOCK:
            caps_lock_active = !caps_lock_active;
            return 0;
        case NUM_LOCK:
            num_lock_active = !num_lock_active;
            set_keyboard_leds();
            return 0;
        case SCROLL_LOCK:
            scroll_lock_active = !scroll_lock_active;
            set_keyboard_leds();
            return 0;
        case BACKSPACE:
            return '\b';
        case ENTER:
            return '\n';
        default:;
    }

    // handle character mapping
    const uint8_t effective_shift = shift_active ^ caps_lock_active;
    char result = 0;

    if (scancode < sizeof(scancode_map_normal)) {
        if (effective_shift) {
            result = scancode_map_shifted[scancode];
        } else {
            result = scancode_map_normal[scancode];
        }
    } else {
        panic("keyboard: invalid scancode");
    }

    return result;
}

char getchark() {
    while (1) {
        const uint8_t scancode = read_scancode();
        const char c = scancode_to_ascii(scancode);
        if (c != 0)
            return c;
    }
}
