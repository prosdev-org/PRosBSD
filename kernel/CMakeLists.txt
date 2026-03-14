list(
        APPEND SUBDIRS
        arch
        drivers
        libk
        libkxx
        machine
        sys
)

add_library(kernel-flags INTERFACE)
add_library(kernel-includes INTERFACE)

target_compile_options(
        kernel-flags INTERFACE
        -O3
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
    target_link_libraries(${subdir} PUBLIC kernel-flags kernel-includes)
    list(APPEND KERNEL_TARGET_OBJECTS $<TARGET_OBJECTS:${subdir}>)
endforeach ()

include(contrib/printf.cmake)

add_library(
        kernel STATIC
        ${KERNEL_TARGET_OBJECTS}
)
