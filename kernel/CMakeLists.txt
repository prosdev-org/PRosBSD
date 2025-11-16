add_subdirectory(arch)
add_subdirectory(core)
add_subdirectory(libk)

add_library(
        kernel STATIC
        $<TARGET_OBJECTS:arch>
        $<TARGET_OBJECTS:core>
        $<TARGET_OBJECTS:libk>
)

add_compile_options(kernel PRIVATE
        -ffreestanding
        -fno-stack-protector
        -fno-omit-frame-pointer
        -nostdinc
        -Wall
        -Wextra
        -Werror
)

add_compile_options(kernel PRIVATE
        $<$<COMPILE_LANGUAGE:CXX>:
        -fno-exceptions
        -fno-rtti
        -fno-threadsafe-statics
        >
)

add_link_options(kernel PRIVATE
        -nostdlib
        -Wl,--build-id=none
)

