set(CMAKE_OBJCOPY_FLAGS -R .pdr -R .comment -R.note -S -O binary)

add_subdirectory(libarch)
add_subdirectory(boot)
add_subdirectory(image)
