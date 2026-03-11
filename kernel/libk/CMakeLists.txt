add_library(
        libk OBJECT
        internal/new_delete.cxx
        stdio.cxx
        stdlib/heap.c
        string.c
        string_v.c
)

target_include_directories(libk PUBLIC "${KERNEL_INCLUDES}")
