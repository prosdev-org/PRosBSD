.macro ISR_NO_ERR num
    .extern Arch_I386_Interrupts_Isr_\num\()
    .global Arch_I386_Interrupts_Isr_\num\()_stub
    Arch_I386_Interrupts_Isr_\num\()_stub:
        pusha
        cld
        call Arch_I386_Interrupts_Isr_\num\()
        popa
        iret
.endm

.macro ISR_ERR num
    .extern Arch_I386_Interrupts_Isr_\num\()
    .global Arch_I386_Interrupts_Isr_\num\()_stub
    Arch_I386_Interrupts_Isr_\num\()_stub:
        pusha
        cld
        call Arch_I386_Interrupts_Isr_\num\()
        popa
        add $4, %esp
        iret
.endm

.macro ISR_PIC8259 num
    .extern Arch_I386_Interrupts_Isr_Pic8259_\num\()
    .global Arch_I386_Interrupts_Isr_Pic8259_\num\()_stub
    Arch_I386_Interrupts_Isr_Pic8259_\num\()_stub:
        pusha
        cld
        call Arch_I386_Interrupts_Isr_Pic8259_\num\()
        popa
        iret
.endm

ISR_NO_ERR 0
ISR_NO_ERR 1
ISR_NO_ERR 2
ISR_NO_ERR 3
ISR_NO_ERR 4
ISR_NO_ERR 5
ISR_NO_ERR 6
ISR_NO_ERR 7
ISR_ERR 8
ISR_NO_ERR 9
ISR_ERR 10
ISR_ERR 11
ISR_ERR 12
ISR_ERR 13
ISR_ERR 14
ISR_NO_ERR 15
ISR_NO_ERR 16
ISR_ERR 17
ISR_NO_ERR 18
ISR_NO_ERR 19
ISR_NO_ERR 20
ISR_ERR 21

ISR_PIC8259 0
ISR_PIC8259 1
ISR_PIC8259 2
ISR_PIC8259 3
ISR_PIC8259 4
ISR_PIC8259 5
ISR_PIC8259 6
ISR_PIC8259 7
ISR_PIC8259 8
ISR_PIC8259 9
ISR_PIC8259 10
ISR_PIC8259 11
ISR_PIC8259 12
ISR_PIC8259 13
ISR_PIC8259 14
ISR_PIC8259 15
