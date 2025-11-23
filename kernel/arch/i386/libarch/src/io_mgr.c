#include <arch/io_mgr.h>
#include <libarch/io_mgr/vga_text_buffer.h>

static uint16_t ports[ArchIoMgr_ports_enum_SIZE];
static ArchIoMgr_mapped_buffer *mapped_buffers[ArchIoMgr_mapped_buffers_enum_SIZE];

void ArchIoMgr_init() {
    mapped_buffers[ArchIoMgr_mapped_buffer_VGA_TEXT] = ArchIoMgrVgaTextBuffer;
}

uint8_t ArchIoMgr_port_read8(ArchIoMgr_ports_enum port) {
    uint8_t value;
    __asm__ volatile("inb %1, %0" : "=a"(value) : "Nd"(ports[port]));
    return value;
}

uint16_t ArchIoMgr_port_read16(ArchIoMgr_ports_enum port) {
    uint16_t value;
    __asm__ volatile("inw %1, %0" : "=a"(value) : "Nd"(ports[port]));
    return value;
}

uint32_t ArchIoMgr_port_read32(ArchIoMgr_ports_enum port) {
    uint32_t value;
    __asm__ volatile("inl %1, %0" : "=a"(value) : "Nd"(ports[port]));
    return value;
}

void ArchIoMgr_port_write8(ArchIoMgr_ports_enum port, uint8_t value) {
    __asm__ volatile("outb %0, %1" : : "a"(value), "Nd"(ports[port]));
}

void ArchIoMgr_port_write16(ArchIoMgr_ports_enum port, uint16_t value) {
    __asm__ volatile("outw %0, %1" : : "a"(value), "Nd"(ports[port]));
}

void ArchIoMgr_port_write32(ArchIoMgr_ports_enum port, uint32_t value) {
    __asm__ volatile("outl %0, %1" : : "a"(value), "Nd"(ports[port]));
}

size_t ArchIoMgr_mapped_buffer_write(const ArchIoMgr_mapped_buffers_enum buffer, const void *source, const size_t len,
                                     const size_t offset) {
    return mapped_buffers[buffer]->write(mapped_buffers[buffer], source, len, offset);
}

size_t ArchIoMgr_mapped_buffer_read(const ArchIoMgr_mapped_buffers_enum buffer, void *destination, const size_t len,
                                    const size_t offset) {
    return mapped_buffers[buffer]->read(mapped_buffers[buffer], destination, len, offset);
}
