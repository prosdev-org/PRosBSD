add_library(
        arch-i386 STATIC
        src/arch_init.cxx
        src/drivers/vga_text.c
)

target_include_directories(arch-i386 PUBLIC include)
target_link_libraries(arch-i386 PUBLIC arch core libk)
