add_library(
        libkxx OBJECT
        internal/cxa_pure_virtual.cxx
        internal/new_delete.cxx
)

target_include_directories(libkxx PUBLIC "${KERNEL_INCLUDES}")
