set(CMAKE_OBJCOPY_FLAGS -R .pdr -R .comment -R.note -S -O binary)

add_library(
        arch OBJECT
        dummy.c
)

add_subdirectory(boot)
add_subdirectory(image)
