#ifndef IO_H
#define IO_H

#include <stdint.h>

static void io_delay(void) {
    __asm__ volatile("outb %%al, $0x80" : : "a"(0));
}

static void outb(uint8_t value, uint16_t port) {
    __asm__ volatile("outb %0, %1" : : "a"(value), "d"(port));
}

static void outw(uint16_t value, uint16_t port) {
    __asm__ volatile("outw %0, %1" : : "a"(value), "d"(port));
}

static void outl(uint32_t value, uint16_t port) {
    __asm__ volatile("outl %0, %1" : : "a"(value), "d"(port));
}

static uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile("inb %1, %0" : "=a"(ret) : "d"(port));
    return ret;
}

static uint16_t inw(uint16_t port) {
    uint16_t ret;
    __asm__ volatile("inw %1, %0" : "=a"(ret) : "d"(port));
    return ret;
}

static uint32_t inl(uint16_t port) {
    uint32_t ret;
    __asm__ volatile("inl %1, %0" : "=a"(ret) : "d"(port));
    return ret;
}

static void outb_p(uint8_t value, uint16_t port) {
    outb(value, port);
    io_delay();
}


static uint8_t inb_p(uint16_t port) {
    uint8_t ret = inb(port);
    io_delay();
    return ret;
}

#endif
