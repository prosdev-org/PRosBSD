add_library(
        arch-i386 OBJECT
        cpu/gdt.cxx
        cpu/gdt.S
        interrupts/idt.cxx
        interrupts/isr.cxx
        interrupts/isr_stubs.S
        interrupts/pic8259.cxx
)
