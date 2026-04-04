add_custom_target(grub2_image
        DEPENDS kernel.elf
        COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/iso
        COMMAND rm -rf ${CMAKE_CURRENT_BINARY_DIR}/iso/*
        COMMAND cp ${CMAKE_CURRENT_BINARY_DIR}/../kernel.elf ${CMAKE_CURRENT_BINARY_DIR}/iso
        COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/iso/boot/grub
        COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/grub.cfg ${CMAKE_CURRENT_BINARY_DIR}/iso/boot/grub
        COMMAND grub2-mkrescue -o ${CMAKE_BINARY_DIR}/${IMAGE_NAME} ${CMAKE_CURRENT_BINARY_DIR}/iso
        COMMAND ln -sf ${IMAGE_NAME} ${CMAKE_BINARY_DIR}/${IMAGE_LINK_NAME}
        COMMENT "image_i386_multiboot2_grub2: Building CD-ROM image"
)
