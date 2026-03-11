add_library(
        sys OBJECT
        console/video_console.cxx
        kernel.cxx
)

target_include_directories(sys PUBLIC "${KERNEL_INCLUDES}")
