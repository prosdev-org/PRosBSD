add_subdirectory(contrib/printf)
target_link_libraries(printf kernel-flags kernel-includes)
set(
        KERNEL_TARGET_OBJECTS
        ${KERNEL_TARGET_OBJECTS}
        $<TARGET_OBJECTS:printf>
)
