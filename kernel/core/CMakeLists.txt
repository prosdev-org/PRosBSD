add_library(
        core STATIC
        src/init.c
)

target_include_directories(core PUBLIC include)
target_link_libraries(core PUBLIC arch)
