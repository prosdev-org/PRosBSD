#ifndef ARCH_IO_MGR_H
#define ARCH_IO_MGR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

typedef enum {
    ArchIoMgr_ports_enum_SIZE // Have to be at the end of the enum
} ArchIoMgr_ports_enum;

typedef struct ArchIoMgr_mapped_buffer {
    // mapped buffer, destination/source, length, offset
    size_t (*read)(struct ArchIoMgr_mapped_buffer *, void *restrict, size_t, size_t);
    size_t (*write)(struct ArchIoMgr_mapped_buffer *, const void *restrict, size_t, size_t);
    // mapped buffer, type size, destination offset, source offset, count
    void (*move)(struct ArchIoMgr_mapped_buffer *, size_t, size_t, size_t, size_t);
    void (*flush)(struct ArchIoMgr_mapped_buffer *);
    void *opaque;
} ArchIoMgr_mapped_buffer;

typedef enum {
    ArchIoMgr_mapped_buffer_VGA_TEXT,
    ArchIoMgr_mapped_buffers_enum_SIZE // Have to be at the end of the enum
} ArchIoMgr_mapped_buffers_enum;

void ArchIoMgr_init();

uint8_t ArchIoMgr_port_read8(ArchIoMgr_ports_enum port);

uint16_t ArchIoMgr_port_read16(ArchIoMgr_ports_enum port);

uint32_t ArchIoMgr_port_read32(ArchIoMgr_ports_enum port);

void ArchIoMgr_port_write8(ArchIoMgr_ports_enum port, uint8_t value);

void ArchIoMgr_port_write16(ArchIoMgr_ports_enum port, uint16_t value);

void ArchIoMgr_port_write32(ArchIoMgr_ports_enum port, uint32_t value);

size_t ArchIoMgr_mapped_buffer_write(ArchIoMgr_mapped_buffers_enum buffer, const void *source, size_t len,
                                     size_t offset);

size_t ArchIoMgr_mapped_buffer_read(ArchIoMgr_mapped_buffers_enum buffer, void *destination, size_t len, size_t offset);

#ifdef __cplusplus
}
#endif

#endif
