#include <interrupts/pic.h>
#include <util/io.h>

#define PIC1         0x20
#define PIC2         0xA0
#define PIC1_COMMAND PIC1
#define PIC1_DATA    (PIC1 + 1)
#define PIC2_COMMAND PIC2
#define PIC2_DATA    (PIC2 + 1)

#define ICW1_ICW4      0x01
#define ICW1_SINGLE    0x02
#define ICW1_INTERVAL4 0x04
#define ICW1_LEVEL     0x08
#define ICW1_INIT      0x10

#define ICW4_8086       0x01
#define ICW4_AUTO       0x02
#define ICW4_BUF_SLAVE  0x08
#define ICW4_BUF_MASTER 0x0C
#define ICW4_SFNM       0x10

#define CASCADE_IRQ 2

#define PIC_EOI 0x20

void pic_send_eoi(const uint8_t irq) {
    if (irq >= 8) {
        outb(PIC2_COMMAND, PIC_EOI);
    }

    outb(PIC1_COMMAND, PIC_EOI);
}

void pic_remap(const uint8_t offset1, const uint8_t offset2) {
    outb(ICW1_INIT | ICW1_ICW4, PIC1_COMMAND);
    io_delay();
    outb(ICW1_INIT | ICW1_ICW4, PIC2_COMMAND);
    io_delay();
    outb(offset1, PIC1_DATA);
    io_delay();
    outb(offset2, PIC2_DATA);
    io_delay();
    outb(1 << CASCADE_IRQ, PIC1_DATA);
    io_delay();
    outb(2, PIC2_DATA);
    io_delay();

    outb(ICW4_8086, PIC1_DATA);
    io_delay();
    outb(ICW4_8086, PIC2_DATA);
    io_delay();

    outb(0, PIC1_DATA);
    outb(0, PIC2_DATA);
}
