add_custom_target(syslinux_image
        DEPENDS floppy
        COMMAND dd if=/dev/zero of=${IMAGE_TEMP_STORAGE_PATH}/${IMAGE_NAME} bs=1M count=70
        COMMAND dd if=/dev/zero of=${IMAGE_TEMP_STORAGE_PATH}/1.img bs=1M count=33
        COMMAND dd if=/dev/zero of=${IMAGE_TEMP_STORAGE_PATH}/2.img bs=1M count=34
        COMMAND parted -s ${IMAGE_TEMP_STORAGE_PATH}/${IMAGE_NAME} mklabel msdos
        COMMAND parted -s ${IMAGE_TEMP_STORAGE_PATH}/${IMAGE_NAME} mkpart primary 1MiB 34MiB
        COMMAND parted -s ${IMAGE_TEMP_STORAGE_PATH}/${IMAGE_NAME} mkpart primary 35MiB 69MiB
        COMMAND mkfs.fat -F 32 ${IMAGE_TEMP_STORAGE_PATH}/1.img
        COMMAND mkfs.ext2 ${IMAGE_TEMP_STORAGE_PATH}/2.img
        COMMAND mcopy -i ${IMAGE_TEMP_STORAGE_PATH}/1.img ${MEMDISK_PATH} ::/memdisk
        COMMAND mcopy -i ${IMAGE_TEMP_STORAGE_PATH}/1.img ${IMAGE_TEMP_STORAGE_PATH}/${FLOPPY_IMAGE_NAME} ::/fd.img
        COMMAND parted -s ${IMAGE_TEMP_STORAGE_PATH}/${IMAGE_NAME} set 1 boot on
        COMMAND mcopy -i ${IMAGE_TEMP_STORAGE_PATH}/1.img ${CMAKE_CURRENT_SOURCE_DIR}/syslinux.cfg ::/syslinux.cfg
        COMMAND syslinux --install ${IMAGE_TEMP_STORAGE_PATH}/1.img
        COMMAND dd if=${IMAGE_TEMP_STORAGE_PATH}/1.img of=${IMAGE_TEMP_STORAGE_PATH}/${IMAGE_NAME} bs=1M seek=1 conv=notrunc
        COMMAND dd if=${IMAGE_TEMP_STORAGE_PATH}/2.img of=${IMAGE_TEMP_STORAGE_PATH}/${IMAGE_NAME} bs=1M seek=35 conv=notrunc
        COMMAND parted -s ${IMAGE_TEMP_STORAGE_PATH}/${IMAGE_NAME} unit B print
        COMMAND cp ${IMAGE_TEMP_STORAGE_PATH}/${IMAGE_NAME} ${CMAKE_BINARY_DIR}/${IMAGE_NAME}
        COMMAND ln -sf ${IMAGE_NAME} ${CMAKE_BINARY_DIR}/${IMAGE_LINK_NAME}
        COMMENT "image_i386_syslinux: Building hard disk image"
)
