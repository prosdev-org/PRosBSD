#include <arch/io_mgr.h>
#include <libarch/io_mgr/vga_text_buffer.h>

#define BUFFER_OFFSET (uintptr_t) 0xC00B8000
#define BUFFER_SIZE   sizeof(uint16_t) * 80 * 25

// TODO: Shadow buffer, flush, move

static size_t read(ArchIoMgr_mapped_buffer *buffer, void *restrict destination, const size_t len, const size_t offset) {
    char *dest = destination;
    for (size_t i = 0; i < len; i++) {
        if (BUFFER_OFFSET + offset + i >= BUFFER_OFFSET + BUFFER_SIZE) {
            return i;
        }
        dest[i] = *(char *) (BUFFER_OFFSET + offset + i);
    }
    return len;
}

static size_t write(ArchIoMgr_mapped_buffer *buffer, const void *restrict source, const size_t len,
                    const size_t offset) {
    const char *src = source;
    for (size_t i = 0; i < len; i++) {
        if (BUFFER_OFFSET + offset + i >= BUFFER_OFFSET + BUFFER_SIZE) {
            return i;
        }
        char *target = (char *) (BUFFER_OFFSET + offset + i);
        *target = src[i];
    }
    return len;
}

static ArchIoMgr_mapped_buffer buffer = {&read, &write, NULL, NULL, NULL};

ArchIoMgr_mapped_buffer *ArchIoMgrVgaTextBuffer = &buffer;
