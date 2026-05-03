add_subdirectory(${PROJECT_TARGET_ARCH})

add_library(
        vmem OBJECT
        memory_map.cxx
        page_frame.cxx
        page_frame_allocator.cxx
)
