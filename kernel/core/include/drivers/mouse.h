#ifndef MOUSE_H
#define MOUSE_H

#include <stdint.h>

#define MOUSE_MIDDLE_BTN      0x04
#define MOUSE_RIGHT_BTN       0x02
#define MOUSE_LEFT_BTN        0x01
#define MOUSE_SAMPLE_RATE_10  10
#define MOUSE_SAMPLE_RATE_20  20
#define MOUSE_SAMPLE_RATE_40  40
#define MOUSE_SAMPLE_RATE_60  60
#define MOUSE_SAMPLE_RATE_80  80
#define MOUSE_SAMPLE_RATE_100 100
#define MOUSE_SAMPLE_RATE_200 200
#define MOUSE_RES_1_COUNT_MM  0x00
#define MOUSE_RES_2_COUNT_MM  0x01
#define MOUSE_RES_4_COUNT_MM  0x02
#define MOUSE_RES_8_COUNT_MM  0x03

typedef struct {
    int16_t x;
    int16_t y;
    int8_t z;
    uint8_t buttons;
    uint8_t packet[4];
    uint8_t packet_index;
    uint8_t packet_size;
    uint8_t id;
} mouse_state;

extern mouse_state mouse;

void mouse_init();
void mouse_wait(uint8_t type);
void mouse_write(uint8_t command);
void mouse_poll();
uint8_t mouse_read();
void mouse_handler();
void mouse_parse_packet();
void mouse_set_sample_rate(uint8_t rate);
void mouse_set_resolution(uint8_t res);
uint8_t mouse_get_id();

#endif
