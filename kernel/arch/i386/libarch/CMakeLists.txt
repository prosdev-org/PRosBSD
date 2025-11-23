add_library(
        arch-i386 STATIC
        src/io_mgr.c
        src/io_mgr/vga_text_buffer.c
)

target_include_directories(arch-i386 PUBLIC include)
target_link_libraries(arch-i386 PUBLIC arch)
