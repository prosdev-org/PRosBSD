set(FLOPPY_IMAGE_NAME "fd.img")
set(IMAGE_NAME "prosbsd-v.${PROJECT_VERSION_FULL}.img")
set(IMAGE_LINK_NAME "prosbsd.img")

set(IMAGE_TEMP_STORAGE_PATH "${CMAKE_BINARY_DIR}/image")
set(MEMDISK_PATH "/usr/share/syslinux/memdisk")

set(CMAKE_OBJCOPY_FLAGS --strip-debug -R .pdr -R .comment -R.note -S -O binary)

add_subdirectory(bootloader)
add_subdirectory(syslinux)

add_custom_target(image ALL)
add_dependencies(image syslinux_image)

add_custom_command(
        OUTPUT kernel_bootstrap.o
        COMMAND ${CMAKE_ASM_COMPILER} ${CMAKE_ASM_FLAGS} -c ${CMAKE_CURRENT_SOURCE_DIR}/kernel_bootstrap.S -o ${CMAKE_CURRENT_BINARY_DIR}/kernel_bootstrap.o
        COMMENT "image_i386: Building 32-bit kernel bootstrap"
)

add_custom_target(kernel.elf
        DEPENDS kernel kernel_bootstrap.o
        COMMAND ${CMAKE_LINKER} ${CMAKE_LINKER_FLAGS} -T ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld -o ${CMAKE_CURRENT_BINARY_DIR}/kernel.elf ${CMAKE_CURRENT_BINARY_DIR}/kernel_bootstrap.o ${CMAKE_BINARY_DIR}/kernel/libkernel.a
        COMMENT "image_i386: Linking raw binary KERNEL.BIN"
)

add_custom_target(KERNEL.BIN
        DEPENDS kernel.elf
        COMMAND cp ${CMAKE_CURRENT_BINARY_DIR}/kernel.elf ${CMAKE_CURRENT_BINARY_DIR}/KERNEL.BIN
        COMMAND ${CMAKE_STRIP} ${CMAKE_CURRENT_BINARY_DIR}/KERNEL.BIN
        COMMAND ${CMAKE_OBJCOPY} ${CMAKE_OBJCOPY_FLAGS} ${CMAKE_CURRENT_BINARY_DIR}/KERNEL.BIN
        COMMENT "image_i386: Linking raw binary KERNEL.BIN"
)

add_custom_target(floppy
        DEPENDS KERNEL.BIN bootloader
        COMMAND mkdir -p ${IMAGE_TEMP_STORAGE_PATH}
        COMMAND dd if=/dev/zero of=${IMAGE_TEMP_STORAGE_PATH}/${FLOPPY_IMAGE_NAME} count=2880 bs=512
        COMMAND mkfs.fat -F 12 ${IMAGE_TEMP_STORAGE_PATH}/${FLOPPY_IMAGE_NAME}
        COMMAND dd if=${CMAKE_CURRENT_BINARY_DIR}/bootloader/MBR.BIN of=${IMAGE_TEMP_STORAGE_PATH}/${FLOPPY_IMAGE_NAME} conv=notrunc
        COMMAND mcopy -i ${IMAGE_TEMP_STORAGE_PATH}/${FLOPPY_IMAGE_NAME} ${CMAKE_CURRENT_BINARY_DIR}/bootloader/SETUP.BIN ::/
        COMMAND mcopy -i ${IMAGE_TEMP_STORAGE_PATH}/${FLOPPY_IMAGE_NAME} ${CMAKE_CURRENT_BINARY_DIR}/KERNEL.BIN ::/
        COMMENT "image_i386: Building floppy image fd.img"
)
