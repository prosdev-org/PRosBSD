#include <arch/i386/interrupts/isr.hxx>
#include <sys/panic.hxx>

namespace Arch::I386::Interrupts::Isr {
    void Arch_I386_Interrupts_Isr_0() {
        Sys::panic("ISR: Division Error");
    }

    void Arch_I386_Interrupts_Isr_1() {
        Sys::panic("ISR: Debug");
    }

    void Arch_I386_Interrupts_Isr_2() {
        Sys::panic("ISR: Non-maskable Interrupt");
    }

    void Arch_I386_Interrupts_Isr_3() {
        Sys::panic("ISR: Breakpoint");
    }

    void Arch_I386_Interrupts_Isr_4() {
        Sys::panic("ISR: Overflow");
    }

    void Arch_I386_Interrupts_Isr_5() {
        Sys::panic("ISR: Bound Range Exceeded");
    }

    void Arch_I386_Interrupts_Isr_6() {
        Sys::panic("ISR: Invalid Opcode");
    }

    void Arch_I386_Interrupts_Isr_7() {
        Sys::panic("ISR: Device not Available");
    }

    void Arch_I386_Interrupts_Isr_8() {
        Sys::panic("ISR: Double Fault");
    }

    void Arch_I386_Interrupts_Isr_9() {
        Sys::panic("ISR: Coprocessor Segment Overrun");
    }

    void Arch_I386_Interrupts_Isr_10() {
        Sys::panic("ISR: Invalid TSS");
    }

    void Arch_I386_Interrupts_Isr_11() {
        Sys::panic("ISR: Segment Not Present");
    }

    void Arch_I386_Interrupts_Isr_12() {
        Sys::panic("ISR: Stack-Segment Fault");
    }

    void Arch_I386_Interrupts_Isr_13() {
        Sys::panic("ISR: General Protection Fault");
    }

    void Arch_I386_Interrupts_Isr_14() {
        Sys::panic("ISR: Page Fault");
    }

    void Arch_I386_Interrupts_Isr_15() {
        Sys::panic("ISR: Reserved");
    }

    void Arch_I386_Interrupts_Isr_16() {
        Sys::panic("ISR: x87 Floating-Point Exception");
    }

    void Arch_I386_Interrupts_Isr_17() {
        Sys::panic("ISR: Alignment Check");
    }

    void Arch_I386_Interrupts_Isr_18() {
        Sys::panic("ISR: Machine Check");
    }

    void Arch_I386_Interrupts_Isr_19() {
        Sys::panic("ISR: SIMD Floating-Point Exception");
    }

    void Arch_I386_Interrupts_Isr_20() {
        Sys::panic("ISR: Virtualization Exception");
    }

    void Arch_I386_Interrupts_Isr_21() {
        Sys::panic("ISR: Control Protection Exception");
    }
} // namespace Arch::I386::Interrupts::Isr
