add_library(
        libk STATIC
        src/string.c
)

target_include_directories(libk PUBLIC include)
target_link_libraries(libk PUBLIC arch)
