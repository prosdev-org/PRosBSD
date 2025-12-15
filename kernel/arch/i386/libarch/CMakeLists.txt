add_library(
        arch-i386 STATIC
        src/_.c
)

target_include_directories(arch-i386 PUBLIC include)
target_link_libraries(arch-i386 PUBLIC arch)
