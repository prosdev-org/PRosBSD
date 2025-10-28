#include <panic.h>

void isr_0_handler() {
    panic("Fault: Divide Error");
}

void isr_1_handler() {
    panic("Trap: Debug Exception");
}

void isr_2_handler() {
    panic("Interrupt: NMI Interrupt");
}

void isr_3_handler() {
    panic("Trap: Breakpoint");
}

void isr_4_handler() {
    panic("Trap: Overflow");
}

void isr_5_handler() {
    panic("Fault: Bound range exceeded");
}

void isr_6_handler() {
    panic("Fault: Invalid Opcode");
}

void isr_7_handler() {
    panic("Fault: Device not available");
}

void isr_8_handler() {
    panic("Abort: Double fault");
}

void isr_9_handler() {
    panic("Fault: Coprocessor segment overrun");
}

void isr_10_handler() {
    panic("Fault: Invalid TSS");
}

void isr_11_handler() {
    panic("Fault: Segment not present");
}

void isr_12_handler() {
    panic("Fault: Stack-segment fault");
}

void isr_13_handler() {
    panic("Fault: General protection");
}

void isr_14_handler() {
    panic("Fault: Page fault");
}

void isr_15_handler() {
    panic("Intel reserved interrupt");
}

void isr_16_handler() {
    panic("Fault: x87 error");
}

void isr_17_handler() {
    panic("Fault: Alignment check");
}

void isr_18_handler() {
    panic("Abort: Machine check");
}

void isr_19_handler() {
    panic("Fault: SIMD floating-point exception");
}

void isr_20_handler() {
    panic("Fault: Virtualization exception");
}

void isr_21_handler() {
    panic("Fault: Control protection exception");
}

void isr_unhandled() {
    panic("Unhandled interrupt");
}
