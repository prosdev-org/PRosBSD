#include "pata_pio.h"
#include <extrns.h>
#include <io.h>
#include <pata_pio.h>
#include <stddef.h>
#include <stdint.h>

static void io_wait() {
    inb(0x3F6);
    inb(0x3F6);
    inb(0x3F6);
    inb(0x3F6);
}

static bool wait_bsy() {
    for (int i = 0; i < 1000000; i++) {
        if (!(inb(PATA_PIO_REG_STATUS) & PATA_PIO_STATUS_BSY)) {
            return true;
        }
    }

    return false;
}

static bool wait_drq() {
    for (int i = 0; i < 1000000; i++) {
        const uint8_t status = inb(PATA_PIO_REG_STATUS);
        if (status & PATA_PIO_STATUS_ERR) {
            return false;
        }
        if (status & PATA_PIO_STATUS_DRQ) {
            return true;
        }
    }

    return false;
}

// temp stub
void pata_pio_init() {
}

bool pata_pio_read_sector(const uint32_t lba, uint8_t *buffer) {
    if (!wait_bsy()) {
        return false;
    }

    outb((uint8_t) (PATA_PIO_MASTER | (lba >> 24) & 0x0F), PATA_PIO_REG_DRIVE);
    io_wait();

    outb(1, PATA_PIO_REG_SECTOR_COUNT);
    outb((uint8_t) (lba & 0xFF), PATA_PIO_REG_LBA_LOW);
    outb((uint8_t) ((lba >> 8) & 0xFF), PATA_PIO_REG_LBA_MID);
    outb((uint8_t) ((lba >> 16) & 0xFF), PATA_PIO_REG_LBA_HIGH);

    outb(PATA_PIO_CMD_READ_SECTORS, PATA_PIO_REG_COMMAND);

    if (!wait_bsy()) {
        return false;
    }

    if (!wait_drq()) {
        return false;
    }

    for (size_t i = 0; i < 256; i++) {
        const uint16_t data = inw(PATA_PIO_REG_DATA);
        buffer[2 * i] = data & 0xFF;
        buffer[2 * i + 1] = (data >> 8) & 0xFF;
    }

    return true;
}

bool pata_pio_write_sector(const uint32_t lba, const uint8_t *buffer) {
    if (!wait_bsy()) {
        return false;
    }

    outb((uint8_t) (PATA_PIO_MASTER | (lba >> 24) & 0x0F), PATA_PIO_REG_DRIVE);
    io_wait();

    outb(1, PATA_PIO_REG_SECTOR_COUNT);
    outb((uint8_t) (lba & 0xFF), PATA_PIO_REG_LBA_LOW);
    outb((uint8_t) ((lba >> 8) & 0xFF), PATA_PIO_REG_LBA_MID);
    outb((uint8_t) ((lba >> 16) & 0xFF), PATA_PIO_REG_LBA_HIGH);

    outb(PATA_PIO_CMD_WRITE_SECTORS, PATA_PIO_REG_COMMAND);

    if (!wait_bsy()) {
        return false;
    }

    if (!wait_drq()) {
        return false;
    }

    for (size_t i = 0; i < 256; i++) {
        outw(((uint16_t) buffer[2 * i] | ((uint16_t) buffer[2 * i + 1] << 8)), PATA_PIO_REG_DATA);
    }

    return true;
}
