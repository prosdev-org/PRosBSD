#include <arch/io_mgr.h>

static uint16_t ports[ArchIoMgr_port_t_ENUM_SIZE];

void ArchIoMgr_init() {
}

uint8_t ArchIoMgr_port_read8(ArchIoMgr_port_t port) {
    uint8_t value;
    __asm__ volatile("inb %1, %0" : "=a"(value) : "Nd"(ports[port]));
    return value;
}

uint16_t ArchIoMgr_port_read16(ArchIoMgr_port_t port) {
    uint16_t value;
    __asm__ volatile("inw %1, %0" : "=a"(value) : "Nd"(ports[port]));
    return value;
}

uint32_t ArchIoMgr_port_read32(ArchIoMgr_port_t port) {
    uint32_t value;
    __asm__ volatile("inl %1, %0" : "=a"(value) : "Nd"(ports[port]));
    return value;
}

void ArchIoMgr_port_write8(ArchIoMgr_port_t port, uint8_t value) {
    __asm__ volatile("outb %0, %1" : : "a"(value), "Nd"(ports[port]));
}

void ArchIoMgr_port_write16(ArchIoMgr_port_t port, uint16_t value) {
    __asm__ volatile("outw %0, %1" : : "a"(value), "Nd"(ports[port]));
}

void ArchIoMgr_port_write32(ArchIoMgr_port_t port, uint32_t value) {
    __asm__ volatile("outl %0, %1" : : "a"(value), "Nd"(ports[port]));
}
