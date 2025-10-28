include_directories(
        ${CMAKE_BINARY_DIR}/include
)

add_subdirectory(boot)
add_subdirectory(core)
add_subdirectory(libk)

set(LINKER_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/linker.ld")

set(CMAKE_EXE_LINKER_FLAGS "-nostdlib -nodefaultlibs -Wl,${CMAKE_LINKER_FLAGS}")

add_library(kernel STATIC
        $<TARGET_OBJECTS:core>
        $<TARGET_OBJECTS:libk>
)

add_custom_target(KERNEL.BIN
        DEPENDS kernel
        COMMAND ${CMAKE_LINKER} ${CMAKE_LINKER_FLAGS} -T ${LINKER_SCRIPT} -o ${CMAKE_CURRENT_BINARY_DIR}/KERNEL.BIN ${CMAKE_CURRENT_BINARY_DIR}/boot/KERNEL_ENTRY.o ${CMAKE_CURRENT_BINARY_DIR}/libkernel.a
        COMMAND ${CMAKE_STRIP} ${CMAKE_CURRENT_BINARY_DIR}/KERNEL.BIN
        COMMAND ${CMAKE_OBJCOPY} ${CMAKE_OBJCOPY_FLAGS} ${CMAKE_CURRENT_BINARY_DIR}/KERNEL.BIN
        COMMENT "Linking raw binary image KERNEL.BIN"
)

add_dependencies(KERNEL.BIN boot)
