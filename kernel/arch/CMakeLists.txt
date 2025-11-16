add_subdirectory(${TARGET_ARCH})
add_library(arch STATIC
        $<TARGET_OBJECTS:arch-${TARGET_ARCH}>
)

target_include_directories(arch INTERFACE include)
