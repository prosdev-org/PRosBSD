add_library(
        libkxx OBJECT
        internal/new_delete.cxx
)

target_include_directories(libkxx PUBLIC "${KERNEL_INCLUDES}")
