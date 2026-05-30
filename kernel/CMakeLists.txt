list(
        APPEND SUBDIRS
        arch
        contrib
        devices
        libk
        libkxx
        machine
        multiboot2
        shell
        sys
        vfs
        vmem
)

list(
        APPEND OBJLIBS
        arch-${PROJECT_TARGET_ARCH}
        contrib-arith64
        contrib-o1heap
        devices
        devices-${PROJECT_TARGET_ARCH}
        libk
        libkxx
        machine-${PROJECT_TARGET_ARCH}
        multiboot2-${PROJECT_TARGET_ARCH}
        shell
        sys
        vfs
        vmem
        vmem-${PROJECT_TARGET_ARCH}
)

add_library(kernel-flags INTERFACE)
add_library(kernel-includes INTERFACE)

target_compile_options(
        kernel-flags INTERFACE
        -O0
        -g
        -march=i486
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

target_include_directories(
        kernel-includes INTERFACE
        "${CMAKE_SOURCE_DIR}/kernel/include"
        "${CMAKE_SOURCE_DIR}/kernel/include/arch/${PROJECT_TARGET_ARCH}/archx"
)

set(KERNEL_TARGET_OBJECTS "")

foreach (subdir IN LISTS SUBDIRS)
    add_subdirectory(${subdir})
endforeach ()

foreach (objlib IN LISTS OBJLIBS)
    target_link_libraries(${objlib} PUBLIC kernel-flags kernel-includes)
    list(APPEND KERNEL_TARGET_OBJECTS $<TARGET_OBJECTS:${objlib}>)
endforeach ()

add_library(
        kernel STATIC
        ${KERNEL_TARGET_OBJECTS}
)
