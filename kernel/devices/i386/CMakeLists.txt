add_library(
        devices-i386 OBJECT
        auto_conf/registry.cxx
        isa_bus.cxx
        vga_text.cxx
)
