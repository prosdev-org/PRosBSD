.macro ISR_NOERR num
    .extern isr_\num\()_handler
    .global isr_\num
    isr_\num:
        call isr_\num\()_handler
        iret
.endm

.macro ISR_ERR num
    .extern isr_\num\()_handler
    .global isr_\num
    isr_\num:
        call isr_\num\()_handler
        add $4, %esp
        iret
.endm

.macro IRQ_STUB num
    .extern irq_\num\()_handler
    .global irq_\num
    irq_\num:
        call irq_\num\()_handler
        iret
.endm

ISR_NOERR 0
ISR_NOERR 1
ISR_NOERR 2
ISR_NOERR 3
ISR_NOERR 4
ISR_NOERR 5
ISR_NOERR 6
ISR_NOERR 7
ISR_ERR   8  # double fault
ISR_NOERR 9
ISR_ERR   10 # invalid TSS
ISR_ERR   11 # segment Not present
ISR_ERR   12 # stack-segment fault
ISR_ERR   13 # general protection fault
ISR_ERR   14 # page fault
ISR_NOERR 15
ISR_NOERR 16
ISR_ERR   17 # alignment check
ISR_NOERR 18
ISR_NOERR 19
ISR_NOERR 20
ISR_ERR   21 # control protection exception

.extern isr_unhandled
.global isr_unhandled_stub
isr_unhandled_stub:
    call isr_unhandled
    iret

IRQ_STUB 0
IRQ_STUB 1
IRQ_STUB 2
IRQ_STUB 3
IRQ_STUB 4
IRQ_STUB 5
IRQ_STUB 6
IRQ_STUB 7
IRQ_STUB 8
IRQ_STUB 9
IRQ_STUB 10
IRQ_STUB 11
IRQ_STUB 12
IRQ_STUB 13
IRQ_STUB 14
IRQ_STUB 15