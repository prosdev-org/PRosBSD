#include <drivers/mouse.h>
#include <stdint.h>
#include <util/io.h>

#define PS2_CMD_WRITE_PORT2   0xD4
#define PS2_CMD_DISABLE_PORT2 0xA7
#define PS2_CMD_ENABLE_PORT2  0xA8
#define MOUSE_CMD_RESET       0xFF
#define MOUSE_CMD_ENABLE      0xF4
#define MOUSE_CMD_DISABLE     0xF5
#define MOUSE_CMD_SET_SAMPLE  0xF3
#define MOUSE_CMD_SET_WRAP    0xEE
#define MOUSE_CMD_SET_RES     0xE8
#define MOUSE_CMD_RESET_WRAP  0xEC
#define MOUSE_CMD_GET_ID      0xF2
#define MOUSE_CMD_SET_REMOTE  0xF0
#define MOUSE_CMD_SET_STREAM  0xEA
#define MOUSE_ACK             0xFA
#define MOUSE_RESEND          0xFE
#define MOUSE_ERROR           0xFC
#define MOUSE_Y_OVERFLOW      0x80
#define MOUSE_X_OVERFLOW      0x40
#define MOUSE_Y_SIGN          0x20
#define MOUSE_X_SIGN          0x10
#define MOUSE_ALWAYS_1        0x08
#define MOUSE_ID_STANDARD     0x00
#define MOUSE_ID_SCROLL       0x03
#define MOUSE_ID_5BUTTON      0x04
#define PS2_DATA_PORT         0x60
#define PS2_STATUS_PORT       0x64
#define PS2_CTRL_PORT         0x64
#define STATUS_OUTPUT_FULL    0x01
#define STATUS_INPUT_FULL     0x02

mouse_state mouse;

void mouse_init() {
    outb(PS2_CMD_DISABLE_PORT2, PS2_CTRL_PORT);
    mouse_wait(1);

    // enable auxiliary
    outb(PS2_CMD_ENABLE_PORT2, PS2_CTRL_PORT);
    mouse_wait(1);

    mouse_write(MOUSE_CMD_RESET);
    mouse_read();
    mouse_read();
    mouse_read();

    mouse_set_sample_rate(MOUSE_SAMPLE_RATE_200);
    mouse_set_sample_rate(MOUSE_SAMPLE_RATE_100);
    mouse_set_sample_rate(MOUSE_SAMPLE_RATE_80);
    const uint8_t id = mouse_get_id();

    // set packet size based on mouse ID
    if (id == MOUSE_ID_SCROLL) {
        mouse.packet_size = 4;
    } else if (id == MOUSE_ID_5BUTTON) {
        mouse.packet_size = 4;
    } else {
        mouse.packet_size = 3;
    }

    // make it default
    mouse_set_resolution(MOUSE_RES_4_COUNT_MM);
    mouse_set_sample_rate(MOUSE_SAMPLE_RATE_100);

    // data reporting
    mouse_write(MOUSE_CMD_ENABLE);
    mouse_read();

    mouse.x = 0;
    mouse.y = 0;
    mouse.z = 0;
    mouse.buttons = 0;
    mouse.packet_index = 0;
    mouse.id = id;
}

void mouse_wait(const uint8_t type) {
    uint32_t timeout = 100000;
    while (timeout--) {
        const uint8_t status = inb(PS2_STATUS_PORT);

        if (type == 0 && !(status & STATUS_INPUT_FULL))
            return;
        if (type == 1 && (status & STATUS_OUTPUT_FULL))
            return;
    }
}

void mouse_write(const uint8_t command) {
    outb(PS2_CMD_WRITE_PORT2, PS2_CTRL_PORT);
    mouse_wait(1);

    outb(command, PS2_DATA_PORT);
    mouse_wait(1);
}

uint8_t mouse_read() {
    mouse_wait(0);
    return inb(PS2_DATA_PORT);
}

void mouse_handler() {
    const uint8_t status = inb(PS2_STATUS_PORT);

    if (!(status & STATUS_OUTPUT_FULL))
        return;
    if (!(status & 0x20))
        return; // not from mouse

    const uint8_t data = inb(PS2_DATA_PORT);

    if (mouse.packet_index == 0 && (data & MOUSE_ALWAYS_1)) {
        mouse.packet[0] = data;
        mouse.packet_index++;
    } else if (mouse.packet_index > 0) {
        mouse.packet[mouse.packet_index] = data;
        mouse.packet_index++;
    }

    // complete
    if (mouse.packet_index == mouse.packet_size) {
        mouse_parse_packet();
        mouse.packet_index = 0;
    }
}

void mouse_parse_packet() {
    const uint8_t flags = mouse.packet[0];

    mouse.buttons = flags & 0x07;

    // calculate movement
    int16_t x = mouse.packet[1];
    int16_t y = mouse.packet[2];

    // apply sign bits
    if (flags & MOUSE_X_SIGN)
        x |= (int16_t) 0xFF00;
    if (flags & MOUSE_Y_SIGN)
        y |= (int16_t) 0xFF00;

    if (!(flags & (MOUSE_X_OVERFLOW | MOUSE_Y_OVERFLOW))) {
        mouse.x += x;
        mouse.y -= y; // invert Y
    }

    // scroll wheel (Z)(OV)
    if (mouse.packet_size == 4) {
        int8_t z = mouse.packet[3] & 0x0F;
        if (z & 0x08)
            z |= (int8_t) 0xF0;
        mouse.z += z;
    }
}

void mouse_set_sample_rate(const uint8_t rate) {
    mouse_write(MOUSE_CMD_SET_SAMPLE);
    mouse_read();
    mouse_write(rate);
    mouse_read();
}

void mouse_set_resolution(const uint8_t res) {
    mouse_write(MOUSE_CMD_SET_RES);
    mouse_read();
    mouse_write(res);
    mouse_read();
}

uint8_t mouse_get_id() {
    mouse_write(MOUSE_CMD_GET_ID);
    mouse_read();
    return mouse_read();
}

void mouse_poll() {
    const uint8_t status = inb(PS2_STATUS_PORT);

    if ((status & STATUS_OUTPUT_FULL) && (status & 0x20)) {
        const uint8_t data = inb(PS2_DATA_PORT);

        if (mouse.packet_index == 0 && (data & MOUSE_ALWAYS_1)) {
            mouse.packet[0] = data;
            mouse.packet_index++;
        } else if (mouse.packet_index > 0) {
            mouse.packet[mouse.packet_index] = data;
            mouse.packet_index++;
        }

        if (mouse.packet_index == mouse.packet_size) {
            mouse_parse_packet();
            mouse.packet_index = 0;
        }
    }
}
