add_library(
        core STATIC
        src/console/video_console.cxx
        src/kernel.cxx
)

target_include_directories(core PUBLIC include)
target_link_libraries(core PUBLIC arch libk)
