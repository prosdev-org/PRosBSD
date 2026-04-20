add_library(
        libkxx OBJECT
        internal/cxa_atexit.cxx
        internal/cxa_pure_virtual.cxx
        internal/dso_handle.cxx
        internal/new_delete.cxx
        math.cxx
        print.cxx
        string.cxx
        string_view.cxx
)
