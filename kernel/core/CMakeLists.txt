add_library(
        core STATIC
        src/main.c
)

target_include_directories(core PUBLIC include)
