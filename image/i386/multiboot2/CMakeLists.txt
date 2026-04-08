set(IMAGE_NAME "prosbsd-v.${PROJECT_VERSION_FULL}.iso")
set(IMAGE_LINK_NAME "prosbsd.iso")

add_subdirectory(grub2)

add_custom_target(image ALL)
add_dependencies(image grub2_image)

add_custom_command(
        OUTPUT kernel_bootstrap.o
        COMMAND ${CMAKE_ASM_COMPILER} ${CMAKE_ASM_FLAGS} ${target_triple_option} -c ${CMAKE_CURRENT_SOURCE_DIR}/kernel_bootstrap.S -o ${CMAKE_CURRENT_BINARY_DIR}/kernel_bootstrap.o
        COMMENT "image_i386_multiboot2: Building 32-bit kernel bootstrap"
)

add_custom_target(kernel.elf
        DEPENDS kernel kernel_bootstrap.o
        COMMAND ${CMAKE_LINKER} ${CMAKE_LINKER_FLAGS} -T ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld ${CMAKE_CURRENT_BINARY_DIR}/kernel_bootstrap.o ${CMAKE_BINARY_DIR}/kernel/libkernel.a -o ${CMAKE_CURRENT_BINARY_DIR}/kernel.elf
        COMMENT "image_i386_multiboot2: Linking kernel image kernel.elf"
)
