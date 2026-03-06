add_library(
        libk STATIC
        src/internal/new_delete.cxx
        src/stdio.cxx
        src/stdlib/heap.c
        src/string.c
        src/string_v.c
)

target_include_directories(libk PUBLIC include)
target_link_libraries(libk PUBLIC arch core)
