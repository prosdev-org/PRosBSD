add_library(
        devices-i386 OBJECT
        auto_conf/registry.cxx
        i8042.cxx
        i8253.cxx
        isa_bus.cxx
        vga_text.cxx
)
