set(IMAGE_NAME "fd.img")
set(HARD_IMAGE_NAME "prosbsd-v.${PROJECT_VERSION_FULL}.img")
set(HARD_IMAGE_LINK_NAME "prosbsd.img")

set(IMAGES_TEMP_STORAGE_PATH "/tmp/image")
set(MEMDISK_PATH "/usr/lib/syslinux/memdisk")

add_custom_command(
        OUTPUT KERNEL_ENTRY.o
        COMMAND ${CMAKE_ASM_COMPILER} ${CMAKE_ASM_FLAGS} -c ${CMAKE_CURRENT_SOURCE_DIR}/src/kernel_entry.S -o ${CMAKE_CURRENT_BINARY_DIR}/kernel_entry.o
        COMMENT "Building 32-bit kernel entry"
)

add_custom_target(KERNEL.BIN
        DEPENDS kernel KERNEL_ENTRY.o
        COMMAND ${CMAKE_LINKER} ${CMAKE_LINKER_FLAGS} -T ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld -o ${CMAKE_CURRENT_BINARY_DIR}/KERNEL.BIN ${CMAKE_CURRENT_BINARY_DIR}/kernel_entry.o $<TARGET_FILE_DIR:kernel>/$<TARGET_FILE_NAME:kernel>
        COMMAND ${CMAKE_STRIP} ${CMAKE_CURRENT_BINARY_DIR}/KERNEL.BIN
        COMMAND ${CMAKE_OBJCOPY} ${CMAKE_OBJCOPY_FLAGS} ${CMAKE_CURRENT_BINARY_DIR}/KERNEL.BIN
        COMMENT "Linking raw binary image KERNEL.BIN"
)

add_custom_target(floppy
        DEPENDS KERNEL.BIN boot
        COMMAND sudo chown builder:builder ${IMAGES_TEMP_STORAGE_PATH}
        COMMAND dd if=/dev/zero of=${IMAGES_TEMP_STORAGE_PATH}/${IMAGE_NAME} count=2880 bs=512
        COMMAND mkfs.fat -F 12 ${IMAGES_TEMP_STORAGE_PATH}/${IMAGE_NAME}
        COMMAND dd if=${CMAKE_CURRENT_BINARY_DIR}/../boot/MBR.BIN of=${IMAGES_TEMP_STORAGE_PATH}/${IMAGE_NAME} conv=notrunc
        COMMAND mcopy -i ${IMAGES_TEMP_STORAGE_PATH}/${IMAGE_NAME} ${CMAKE_CURRENT_BINARY_DIR}/../boot/SETUP.BIN ::/
        COMMAND mcopy -i ${IMAGES_TEMP_STORAGE_PATH}/${IMAGE_NAME} ${CMAKE_CURRENT_BINARY_DIR}/KERNEL.BIN ::/
        COMMENT "Building floppy image fd.img"
)

add_custom_target(hdd_image_syslinux ALL
        DEPENDS floppy
        COMMAND dd if=/dev/zero of=${IMAGES_TEMP_STORAGE_PATH}/${HARD_IMAGE_NAME} bs=1M count=70
        COMMAND dd if=/dev/zero of=${IMAGES_TEMP_STORAGE_PATH}/1.img bs=1M count=33
        COMMAND dd if=/dev/zero of=${IMAGES_TEMP_STORAGE_PATH}/2.img bs=1M count=34
        COMMAND parted -s ${IMAGES_TEMP_STORAGE_PATH}/${HARD_IMAGE_NAME} mklabel msdos
        COMMAND parted -s ${IMAGES_TEMP_STORAGE_PATH}/${HARD_IMAGE_NAME} mkpart primary 1MiB 34MiB
        COMMAND parted -s ${IMAGES_TEMP_STORAGE_PATH}/${HARD_IMAGE_NAME} mkpart primary 35MiB 69MiB
        COMMAND mkfs.fat -F 32 ${IMAGES_TEMP_STORAGE_PATH}/1.img
        COMMAND mkfs.ext2 ${IMAGES_TEMP_STORAGE_PATH}/2.img
        COMMAND mcopy -i ${IMAGES_TEMP_STORAGE_PATH}/1.img ${MEMDISK_PATH} ::/memdisk
        COMMAND mcopy -i ${IMAGES_TEMP_STORAGE_PATH}/1.img ${IMAGES_TEMP_STORAGE_PATH}/${IMAGE_NAME} ::/fd.img
        COMMAND parted -s ${IMAGES_TEMP_STORAGE_PATH}/${HARD_IMAGE_NAME} set 1 boot on
        COMMAND mcopy -i ${IMAGES_TEMP_STORAGE_PATH}/1.img ${CMAKE_CURRENT_SOURCE_DIR}/syslinux.cfg ::/syslinux.cfg
        COMMAND syslinux --install ${IMAGES_TEMP_STORAGE_PATH}/1.img
        COMMAND dd if=${IMAGES_TEMP_STORAGE_PATH}/1.img of=${IMAGES_TEMP_STORAGE_PATH}/${HARD_IMAGE_NAME} bs=1M seek=1 conv=notrunc
        COMMAND dd if=${IMAGES_TEMP_STORAGE_PATH}/2.img of=${IMAGES_TEMP_STORAGE_PATH}/${HARD_IMAGE_NAME} bs=1M seek=35 conv=notrunc
        COMMAND parted -s ${IMAGES_TEMP_STORAGE_PATH}/${HARD_IMAGE_NAME} unit B print
        COMMAND cp ${IMAGES_TEMP_STORAGE_PATH}/${HARD_IMAGE_NAME} ${CMAKE_BINARY_DIR}/${HARD_IMAGE_NAME}
        COMMAND ln -sf ${HARD_IMAGE_NAME} ${CMAKE_BINARY_DIR}/${HARD_IMAGE_LINK_NAME}
        COMMENT "Building hard disk image (syslinux)"
)
