add_library(
        libk STATIC
        src/string.c
        src/string_v.c
)

target_include_directories(libk PUBLIC include)
target_link_libraries(libk PUBLIC arch)
