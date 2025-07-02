#include <stdint.h>

#define PS2_CMD_WRITE_PORT2  0xD4
#define PS2_CMD_DISABLE_PORT2 0xA7
#define PS2_CMD_ENABLE_PORT2  0xA8
#define MOUSE_CMD_RESET        0xFF
#define MOUSE_CMD_ENABLE       0xF4
#define MOUSE_CMD_DISABLE      0xF5
#define MOUSE_CMD_SET_SAMPLE   0xF3
#define MOUSE_CMD_SET_WRAP     0xEE
#define MOUSE_CMD_SET_RES      0xE8
#define MOUSE_CMD_RESET_WRAP   0xEC
#define MOUSE_CMD_GET_ID       0xF2
#define MOUSE_CMD_SET_REMOTE   0xF0
#define MOUSE_CMD_SET_STREAM   0xEA
#define MOUSE_ACK   0xFA
#define MOUSE_RESEND 0xFE
#define MOUSE_ERROR 0xFC
#define MOUSE_Y_OVERFLOW  0x80
#define MOUSE_X_OVERFLOW  0x40
#define MOUSE_Y_SIGN      0x20
#define MOUSE_X_SIGN      0x10
#define MOUSE_ALWAYS_1    0x08
#define MOUSE_MIDDLE_BTN  0x04
#define MOUSE_RIGHT_BTN   0x02
#define MOUSE_LEFT_BTN    0x01
#define MOUSE_SAMPLE_10   10
#define MOUSE_SAMPLE_20   20
#define MOUSE_SAMPLE_40   40
#define MOUSE_SAMPLE_60   60
#define MOUSE_SAMPLE_80   80
#define MOUSE_SAMPLE_100  100
#define MOUSE_SAMPLE_200  200
#define MOUSE_RES_1_COUNT_MM  0x00
#define MOUSE_RES_2_COUNT_MM  0x01
#define MOUSE_RES_4_COUNT_MM  0x02
#define MOUSE_RES_8_COUNT_MM  0x03
#define MOUSE_ID_STANDARD 0x00
#define MOUSE_ID_SCROLL   0x03
#define MOUSE_ID_5BUTTON  0x04
#define PS2_DATA_PORT     0x60
#define PS2_STATUS_PORT   0x64
#define PS2_CTRL_PORT     0x64
#define STATUS_OUTPUT_FULL  0x01
#define STATUS_INPUT_FULL   0x02

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