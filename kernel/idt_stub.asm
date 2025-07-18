[BITS 32]

%macro DECL_EXTERN_ISR 1
    extern isr_%1_handler
%endmacro

%macro DECL_GLOBAL_ISR 1
    global isr_%1
%endmacro

%assign i 0
%rep 22
    DECL_EXTERN_ISR i
    DECL_GLOBAL_ISR i
    %assign i i+1
%endrep

extern isr_unhandled
global isr_unhandled_stub


%macro DECL_EXTERN_IRQ 1
    extern irq_%1_handler
%endmacro

%macro DECL_GLOBAL_IRQ 1
    global irq_%1
%endmacro

%assign i 0
%rep 16
    DECL_EXTERN_IRQ i
    DECL_GLOBAL_IRQ i
    %assign i i+1
%endrep


isr_0:
    call isr_0_handler
    iret

isr_1:
    call isr_1_handler
    iret

isr_2:
    call isr_2_handler
    iret

isr_3:
    call isr_3_handler
    iret

isr_4:
    call isr_4_handler
    iret

isr_5:
    call isr_5_handler
    iret

isr_6:
    call isr_6_handler
    iret

isr_7:
    call isr_7_handler
    iret

isr_8:
    call isr_8_handler
    add esp, 4
    iret

isr_9:
    call isr_9_handler
    iret

isr_10:
    call isr_10_handler
    add esp, 4
    iret

isr_11:
    call isr_11_handler
    add esp, 4
    iret

isr_12:
    call isr_12_handler
    add esp, 4
    iret

isr_13:
    call isr_13_handler
    add esp, 4
    iret

isr_14:
    call isr_14_handler
    add esp, 4
    iret

isr_15:
    call isr_15_handler
    iret

isr_16:
    call isr_16_handler
    iret

isr_17:
    call isr_17_handler
    add esp, 4
    iret

isr_18:
    call isr_18_handler
    iret

isr_19:
    call isr_19_handler
    iret

isr_20:
    call isr_20_handler
    iret

isr_21:
    call isr_21_handler
    add esp, 4
    iret

isr_unhandled_stub:
    call isr_unhandled
    iret

irq_0:
    call irq_0_handler
    iret

irq_1:
    call irq_1_handler
    iret

irq_2:
    call irq_2_handler
    iret

irq_3:
    call irq_3_handler
    iret

irq_4:
    call irq_4_handler
    iret

irq_5:
    call irq_5_handler
    iret

irq_6:
    call irq_6_handler
    iret

irq_7:
    call irq_7_handler
    iret

irq_8:
    call irq_8_handler
    iret

irq_9:
    call irq_9_handler
    iret

irq_10:
    call irq_10_handler
    iret

irq_11:
    call irq_11_handler
    iret

irq_12:
    call irq_12_handler
    iret

irq_13:
    call irq_13_handler
    iret

irq_14:
    call irq_14_handler
    iret

irq_15:
    call irq_15_handler
    iret
