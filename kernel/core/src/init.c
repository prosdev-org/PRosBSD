#include <arch/io_mgr.h>
_Noreturn void CoreInit_main() {
    // Demo
    ArchIoMgr_init();
    uint16_t buf[80];
    ArchIoMgr_mapped_buffer_read(ArchIoMgr_mapped_buffer_VGA_TEXT, buf, 80 * sizeof(uint16_t), 80 * sizeof(uint16_t));
    ArchIoMgr_mapped_buffer_write(ArchIoMgr_mapped_buffer_VGA_TEXT, buf, 80 * sizeof(uint16_t), 0);
    for (;;)
        ;
}
