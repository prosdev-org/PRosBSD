#include <arch/i386/interrupts/isr.hxx>
#include <arch/i386/interrupts/pic8259.hxx>
#include <machine/interrupts.hxx>
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

    void Arch_I386_Interrupts_Isr_Pic8259_0() {
        Machine::Interrupts::call_handlers(0);
        Pic8259::send_end_of_interrupt(0);
    }

    void Arch_I386_Interrupts_Isr_Pic8259_1() {
        Machine::Interrupts::call_handlers(1);
        Pic8259::send_end_of_interrupt(1);
    }

    void Arch_I386_Interrupts_Isr_Pic8259_2() {
        Machine::Interrupts::call_handlers(2);
        Pic8259::send_end_of_interrupt(2);
    }

    void Arch_I386_Interrupts_Isr_Pic8259_3() {
        Machine::Interrupts::call_handlers(3);
        Pic8259::send_end_of_interrupt(3);
    }

    void Arch_I386_Interrupts_Isr_Pic8259_4() {
        Machine::Interrupts::call_handlers(4);
        Pic8259::send_end_of_interrupt(4);
    }

    void Arch_I386_Interrupts_Isr_Pic8259_5() {
        Machine::Interrupts::call_handlers(5);
        Pic8259::send_end_of_interrupt(5);
    }

    void Arch_I386_Interrupts_Isr_Pic8259_6() {
        Machine::Interrupts::call_handlers(6);
        Pic8259::send_end_of_interrupt(6);
    }

    void Arch_I386_Interrupts_Isr_Pic8259_7() {
        Machine::Interrupts::call_handlers(7);
        Pic8259::send_end_of_interrupt(7);
    }

    void Arch_I386_Interrupts_Isr_Pic8259_8() {
        Machine::Interrupts::call_handlers(8);
        Pic8259::send_end_of_interrupt(8);
    }

    void Arch_I386_Interrupts_Isr_Pic8259_9() {
        Machine::Interrupts::call_handlers(9);
        Pic8259::send_end_of_interrupt(9);
    }

    void Arch_I386_Interrupts_Isr_Pic8259_10() {
        Machine::Interrupts::call_handlers(10);
        Pic8259::send_end_of_interrupt(10);
    }

    void Arch_I386_Interrupts_Isr_Pic8259_11() {
        Machine::Interrupts::call_handlers(11);
        Pic8259::send_end_of_interrupt(11);
    }

    void Arch_I386_Interrupts_Isr_Pic8259_12() {
        Machine::Interrupts::call_handlers(12);
        Pic8259::send_end_of_interrupt(12);
    }

    void Arch_I386_Interrupts_Isr_Pic8259_13() {
        Machine::Interrupts::call_handlers(13);
        Pic8259::send_end_of_interrupt(13);
    }

    void Arch_I386_Interrupts_Isr_Pic8259_14() {
        Machine::Interrupts::call_handlers(14);
        Pic8259::send_end_of_interrupt(14);
    }

    void Arch_I386_Interrupts_Isr_Pic8259_15() {
        Machine::Interrupts::call_handlers(15);
        Pic8259::send_end_of_interrupt(15);
    }
} // namespace Arch::I386::Interrupts::Isr
