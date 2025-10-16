set(CMAKE_C_COMPILER_WORKS ON)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_compile_options(
        -m32
        -march=i386
        -O2
        -g
        -fno-builtin
        -fno-exceptions
        -fno-rtti
        -fno-stack-protector
        -fno-omit-frame-pointer
        -fno-strict-aliasing
        -fno-delete-null-pointer-checks
        -fwrapv
        -fno-tree-vrp
        -fno-reorder-functions
        -fno-reorder-blocks
        -fno-pie
        -fno-pic
        -fno-lto
        -fno-common
        -fno-ident
        -fno-asynchronous-unwind-tables
        -fno-unwind-tables
        -nostdinc
        -nostdlib
        -Wall
        -Wextra
        -Wshadow
        -Wstrict-aliasing
        -Wpointer-arith
        -Wcast-align
        -Wundef
        -Wwrite-strings
        -Wmissing-prototypes
)

set(CMAKE_LINKER_FLAGS -melf_i386)
set(CMAKE_ASM_FLAGS -m32)
set(CMAKE_OBJCOPY_FLAGS -R .pdr -R .comment -R.note -S -O binary)

set(ISGRUBQ FALSE)

set(IMAGE_NAME "fd.img")
set(HARD_IMAGE_NAME "prosbsd-v.${PROJECT_VERSION_FULL}.img")
set(HARD_IMAGE_LINK_NAME "prosbsd.img")

set(MEMDISK_PATH "/usr/lib/syslinux/memdisk")
set(IMAGES_TEMP_STORAGE_PATH "/tmp/image")
