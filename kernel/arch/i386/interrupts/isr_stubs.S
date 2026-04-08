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
