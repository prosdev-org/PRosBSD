add_subdirectory(boot)

file(GLOB_RECURSE KERNEL_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.c" "${CMAKE_CURRENT_SOURCE_DIR}/*.s" "${CMAKE_CURRENT_SOURCE_DIR}/*.S")
# Exclude boot/ from kernel sources. It is separate.
list(FILTER KERNEL_SOURCES EXCLUDE REGEX ".*/boot/.*")

include_directories(
        ${CMAKE_BINARY_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/libk/include
)

set(LINKER_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/linker.ld")

set(CMAKE_EXE_LINKER_FLAGS "-nostdlib -nodefaultlibs -Wl,${CMAKE_LINKER_FLAGS}")

add_library(kernel ${KERNEL_SOURCES})

add_custom_target(KERNEL.BIN
        DEPENDS kernel
        COMMAND ${CMAKE_LINKER} ${CMAKE_LINKER_FLAGS} -T ${LINKER_SCRIPT} -o ${CMAKE_CURRENT_BINARY_DIR}/KERNEL.BIN ${CMAKE_CURRENT_BINARY_DIR}/boot/KERNEL_ENTRY.o ${CMAKE_CURRENT_BINARY_DIR}/libkernel.a
        COMMAND ${CMAKE_STRIP} ${CMAKE_CURRENT_BINARY_DIR}/KERNEL.BIN
        COMMAND ${CMAKE_OBJCOPY} ${CMAKE_OBJCOPY_FLAGS} ${CMAKE_CURRENT_BINARY_DIR}/KERNEL.BIN
        COMMENT "Linking raw binary image KERNEL.BIN"
)

add_dependencies(KERNEL.BIN boot)
