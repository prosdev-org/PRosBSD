#ifndef ARCH_IO_MGR_H
#define ARCH_IO_MGR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef enum {
    ArchIoMgr_port_t_ENUM_SIZE // Have to be at the end of the enum
} ArchIoMgr_port_t;

// typedef enum {
//
// } io_mgr_buffer_t;

void ArchIoMgr_init();

uint8_t ArchIoMgr_port_read8(ArchIoMgr_port_t port);

uint16_t ArchIoMgr_port_read16(ArchIoMgr_port_t port);

uint32_t ArchIoMgr_port_read32(ArchIoMgr_port_t port);

void ArchIoMgr_port_write8(ArchIoMgr_port_t port, uint8_t value);

void ArchIoMgr_port_write16(ArchIoMgr_port_t port, uint16_t value);

void ArchIoMgr_port_write32(ArchIoMgr_port_t port, uint32_t value);

#ifdef __cplusplus
}
#endif

#endif
