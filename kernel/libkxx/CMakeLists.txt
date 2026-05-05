add_library(
        libkxx OBJECT
        bitmap.cxx
        internal/cxa_atexit.cxx
        internal/cxa_pure_virtual.cxx
        internal/dso_handle.cxx
        internal/new_delete.cxx
        input.cxx
        print.cxx
        string.cxx
        string_view.cxx
        to_hex.cxx
)
