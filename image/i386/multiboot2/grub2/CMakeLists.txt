add_custom_target(grub2_iso_image
        DEPENDS kernel.elf
        COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/iso
        COMMAND rm -rf ${CMAKE_CURRENT_BINARY_DIR}/iso/*
        COMMAND cp ${CMAKE_CURRENT_BINARY_DIR}/../kernel.elf ${CMAKE_CURRENT_BINARY_DIR}/iso
        COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/iso/boot/grub
        COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/grub.cfg ${CMAKE_CURRENT_BINARY_DIR}/iso/boot/grub
        COMMAND grub2-mkrescue -o ${CMAKE_BINARY_DIR}/${ISO_IMAGE_NAME} ${CMAKE_CURRENT_BINARY_DIR}/iso
        COMMAND ln -sf ${ISO_IMAGE_NAME} ${CMAKE_BINARY_DIR}/${ISO_IMAGE_LINK_NAME}
        COMMENT "image_i386_multiboot2_grub2: Building ISO image"
)

set(SFDISK_INPUT "${CMAKE_CURRENT_BINARY_DIR}/sfdisk_input")

file(WRITE ${SFDISK_INPUT}
        "label: dos
device: ${CMAKE_BINARY_DIR}/${HDD_IMAGE_NAME}
unit: sectors
${CMAKE_BINARY_DIR}/${HDD_IMAGE_NAME} : start=2048, type=ef, bootable"
)

find_file(GRUB2_BOOT_IMG
        NAMES boot.img
        PATHS
        /usr/lib/grub/i386-pc
        /usr/share/grub/i386-pc
        /lib/grub/i386-pc
)

if (NOT GRUB2_BOOT_IMG)
    message(FATAL_ERROR "Cound not find grub boot.img")
endif ()

add_custom_target(grub2_hdd_image
        DEPENDS kernel.elf
        COMMAND truncate -s 10M ${CMAKE_BINARY_DIR}/${HDD_IMAGE_NAME}
        COMMAND sfdisk ${CMAKE_BINARY_DIR}/${HDD_IMAGE_NAME} < ${SFDISK_INPUT}
        COMMAND truncate -s 9M ${CMAKE_CURRENT_BINARY_DIR}/part.img
        COMMAND mkfs.fat -F16 ${CMAKE_CURRENT_BINARY_DIR}/part.img
        COMMAND grub2-mkimage -O i386-pc
        -o ${CMAKE_CURRENT_BINARY_DIR}/grub2_core.img
        -c ${CMAKE_CURRENT_SOURCE_DIR}/bootstrap.cfg
        -p /boot/grub
        biosdisk part_msdos fat normal search_fs_uuid search test multiboot2
        COMMAND dd
        if=${GRUB2_BOOT_IMG} of=${CMAKE_BINARY_DIR}/${HDD_IMAGE_NAME}
        bs=446 count=1 conv=notrunc
        COMMAND printf '\\x01\\x00\\x00\\x00' |
        dd of=${CMAKE_BINARY_DIR}/${HDD_IMAGE_NAME} bs=1 seek=92 conv=notrunc
        COMMAND dd
        if=${CMAKE_CURRENT_BINARY_DIR}/grub2_core.img of=${CMAKE_BINARY_DIR}/${HDD_IMAGE_NAME}
        bs=512 seek=1 conv=notrunc
        COMMAND dd
        if=${CMAKE_CURRENT_BINARY_DIR}/part.img of=${CMAKE_BINARY_DIR}/${HDD_IMAGE_NAME}
        bs=512 seek=2048 conv=notrunc
        COMMAND mmd -i ${CMAKE_BINARY_DIR}/${HDD_IMAGE_NAME}@@1M ::/boot
        COMMAND mmd -i ${CMAKE_BINARY_DIR}/${HDD_IMAGE_NAME}@@1M ::/boot/grub
        COMMAND mcopy -i ${CMAKE_BINARY_DIR}/${HDD_IMAGE_NAME}@@1M
        ${CMAKE_CURRENT_SOURCE_DIR}/grub.cfg ::/boot/grub/grub.cfg
        COMMAND mcopy -i ${CMAKE_BINARY_DIR}/${HDD_IMAGE_NAME}@@1M
        ${CMAKE_CURRENT_BINARY_DIR}/../kernel.elf ::/kernel.elf
        COMMAND ln -sf ${HDD_IMAGE_NAME} ${CMAKE_BINARY_DIR}/${HDD_IMAGE_LINK_NAME}
        COMMENT "image_i386_multiboot2_grub2: Building HDD image"
)
