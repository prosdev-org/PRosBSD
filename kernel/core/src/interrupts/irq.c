#include <interrupts/pic.h>
#include <timer.h>

void irq_0_handler() {
    timer_tick();
    pic_send_eoi(0);
}

void irq_1_handler() {
    pic_send_eoi(1);
}

void irq_2_handler() {
    pic_send_eoi(2);
}

void irq_3_handler() {
    pic_send_eoi(3);
}

void irq_4_handler() {
    pic_send_eoi(4);
}

void irq_5_handler() {
    pic_send_eoi(5);
}

void irq_6_handler() {
    pic_send_eoi(6);
}

void irq_7_handler() {
    pic_send_eoi(7);
}

void irq_8_handler() {
    pic_send_eoi(8);
}

void irq_9_handler() {
    pic_send_eoi(9);
}

void irq_10_handler() {
    pic_send_eoi(10);
}

void irq_11_handler() {
    pic_send_eoi(11);
}

void irq_12_handler() {
    pic_send_eoi(12);
}

void irq_13_handler() {
    pic_send_eoi(13);
}

void irq_14_handler() {
    pic_send_eoi(14);
}

void irq_15_handler() {
    pic_send_eoi(15);
}
