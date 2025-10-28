#include <drivers/pit.h>
#include <util/io.h>

#define CHANNEL0_DATA_PORT 0x40
#define COMMAND_PORT       0x43

void pit_init() {
    outb(0x36, COMMAND_PORT); // 0x36 - Channel 0, lobyte/hibyte, mode 3, binary mode
    outb(PIT_DIVISOR & 0xFF, CHANNEL0_DATA_PORT);
    outb(PIT_DIVISOR >> 8, CHANNEL0_DATA_PORT);
}
