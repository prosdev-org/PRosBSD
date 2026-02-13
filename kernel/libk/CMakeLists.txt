add_library(
        libk STATIC
        src/stdlib/heap.c
        src/string.c
        src/string_v.c
)

target_include_directories(libk PUBLIC include)
target_link_libraries(libk PUBLIC arch)
