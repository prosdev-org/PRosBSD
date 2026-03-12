list(
        APPEND SUBDIRS
        arch
        drivers
        libk
        libkxx
        machine
        sys
)

set(
        KERNEL_INCLUDES
        "${CMAKE_SOURCE_DIR}/kernel/include"
        "${CMAKE_SOURCE_DIR}/kernel/include/arch/${TARGET_ARCH}/archx"
)

set(KERNEL_SOURCES "")

foreach (subdir IN LISTS SUBDIRS)
    add_subdirectory(${subdir})

    target_compile_options(${subdir} PUBLIC
            -g
            -m32
            -ffreestanding
            -fno-stack-protector
            -fno-omit-frame-pointer
            -nostdinc
            -Wall
            -Wextra
            -Werror
            $<$<COMPILE_LANGUAGE:CXX>:
            -fno-exceptions
            -fno-rtti
            -fno-threadsafe-statics
            >
    )

    list(APPEND KERNEL_SOURCES $<TARGET_OBJECTS:${subdir}>)
endforeach ()

add_library(
        kernel STATIC
        ${KERNEL_SOURCES}
)
