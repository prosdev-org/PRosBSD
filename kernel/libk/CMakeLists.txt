add_library(
        libk OBJECT
        stdio.cxx
        stdlib/heap.c
        string.c
        string_v.c
)

target_include_directories(libk PUBLIC "${KERNEL_INCLUDES}")
