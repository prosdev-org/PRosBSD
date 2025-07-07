#include "mouse.h"
#include <mouse.h>
#include <io.h>
#include <stdint.h>

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
    uint8_t id = mouse_get_id();
    
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

void mouse_wait(uint8_t type) {
    uint32_t timeout = 100000;
    while (timeout--) {
        uint8_t status = inb(PS2_STATUS_PORT);
        
        if (type == 0 && !(status & STATUS_INPUT_FULL)) return;
        if (type == 1 && (status & STATUS_OUTPUT_FULL)) return;
    }
}

void mouse_write(uint8_t command) {
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
    uint8_t status = inb(PS2_STATUS_PORT);
    
    if (!(status & STATUS_OUTPUT_FULL)) return;
    if (!(status & 0x20)) return;  // not from mouse
    
    uint8_t data = inb(PS2_DATA_PORT);
    
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
    uint8_t flags = mouse.packet[0];
    
    mouse.buttons = flags & 0x07;
    
    // calculate movement
    int16_t x = mouse.packet[1];
    int16_t y = mouse.packet[2];
    
    // apply sign bits
    if (flags & MOUSE_X_SIGN) x |= 0xFF00;
    if (flags & MOUSE_Y_SIGN) y |= 0xFF00;
    
    if (!(flags & (MOUSE_X_OVERFLOW | MOUSE_Y_OVERFLOW))) {
        mouse.x += x;
        mouse.y -= y;  // invert Y
    }
    
    // scroll wheel (Z)(OV)
    if (mouse.packet_size == 4) {
        int8_t z = mouse.packet[3] & 0x0F;
        if (z & 0x08) z |= 0xF0;
        mouse.z += z;
    }
}

void mouse_set_sample_rate(uint8_t rate) {
    mouse_write(MOUSE_CMD_SET_SAMPLE);
    mouse_read();
    mouse_write(rate);
    mouse_read();
}

void mouse_set_resolution(uint8_t res) {
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
    uint8_t status = inb(PS2_STATUS_PORT);
    
    if ((status & STATUS_OUTPUT_FULL) && (status & 0x20)) {
        uint8_t data = inb(PS2_DATA_PORT);
        
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