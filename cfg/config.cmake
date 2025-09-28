set(CMAKE_C_COMPILER_WORKS ON)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_compile_options(
    -O2
    -c
    -g
    -m32
    -fno-builtin
    -fno-stack-protector
    -fno-omit-frame-pointer
    -fno-inline
    -fstrength-reduce
    -fno-pic
    -nostdinc
    -nostdlib
    -Wall
    -Wextra
    -Wshadow
    -Wstrict-aliasing
    -w
)

set(CMAKE_LINKER_FLAGS -melf_i386)
set(CMAKE_ASM_FLAGS -m32)
set(CMAKE_OBJCOPY_FLAGS -R .pdr -R .comment -R.note -S -O binary)

set(ISGRUBQ FALSE)

set(IMAGE_NAME "fd.img")
set(HARD_IMAGE_NAME "prosbsd-v.${PROJECT_VERSION_FULL}.img")
set(HARD_IMAGE_LINK_NAME "prosbsd.img")

set(MEMDISK_PATH "/usr/lib/syslinux/memdisk")
