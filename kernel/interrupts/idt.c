#include <interrupts/idt.h>
#include <interrupts/pic.h>
#include <stdint.h>

typedef struct {
    uint16_t isr_low;
    uint16_t kernel_cs;
    uint8_t reserved;
    uint8_t attributes;
    uint16_t isr_high;
} __attribute__((packed)) idt_entry_t;

typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) idtr_t;

__attribute((aligned(0x10))) static idt_entry_t idt[256];

static idtr_t idtr;

void idt_set_descriptor(uint8_t vector, void *isr, uint8_t flags) {
    idt_entry_t *descriptor = &idt[vector];

    descriptor->isr_low = (uint32_t) isr & 0xFFFF;
    descriptor->kernel_cs = 0x08;
    descriptor->attributes = flags;
    descriptor->isr_high = (uint32_t) isr >> 16;
    descriptor->reserved = 0;
}

extern void isr_0();
extern void isr_1();
extern void isr_2();
extern void isr_3();
extern void isr_4();
extern void isr_5();
extern void isr_6();
extern void isr_7();
extern void isr_8();
extern void isr_9();
extern void isr_10();
extern void isr_11();
extern void isr_12();
extern void isr_13();
extern void isr_14();
extern void isr_15();
extern void isr_16();
extern void isr_17();
extern void isr_18();
extern void isr_19();
extern void isr_20();
extern void isr_21();
extern void isr_unhandled_stub();

extern void irq_0();
extern void irq_1();
extern void irq_2();
extern void irq_3();
extern void irq_4();
extern void irq_5();
extern void irq_6();
extern void irq_7();
extern void irq_8();
extern void irq_9();
extern void irq_10();
extern void irq_11();
extern void irq_12();
extern void irq_13();
extern void irq_14();
extern void irq_15();

void idt_init() {
    idtr.base = (uintptr_t) &idt[0];
    idtr.limit = (uint16_t) sizeof(idt_entry_t) * 256 - 1;

    for (int i = 0; i < 256; i++) {
        idt_set_descriptor(i, isr_unhandled_stub, 0x8E);
    }

    idt_set_descriptor(0, isr_0, 0x8E);
    idt_set_descriptor(1, isr_1, 0x8E);
    idt_set_descriptor(2, isr_2, 0x8E);
    idt_set_descriptor(3, isr_3, 0x8E);
    idt_set_descriptor(4, isr_4, 0x8E);
    idt_set_descriptor(5, isr_5, 0x8E);
    idt_set_descriptor(6, isr_6, 0x8E);
    idt_set_descriptor(7, isr_7, 0x8E);
    idt_set_descriptor(8, isr_8, 0x8E);
    idt_set_descriptor(9, isr_9, 0x8E);
    idt_set_descriptor(10, isr_10, 0x8E);
    idt_set_descriptor(11, isr_11, 0x8E);
    idt_set_descriptor(12, isr_12, 0x8E);
    idt_set_descriptor(13, isr_13, 0x8E);
    idt_set_descriptor(14, isr_14, 0x8E);
    idt_set_descriptor(15, isr_15, 0x8E);
    idt_set_descriptor(16, isr_16, 0x8E);
    idt_set_descriptor(17, isr_17, 0x8E);
    idt_set_descriptor(18, isr_18, 0x8E);
    idt_set_descriptor(19, isr_19, 0x8E);
    idt_set_descriptor(20, isr_20, 0x8E);
    idt_set_descriptor(21, isr_21, 0x8E);

    pic_remap(0x20, 0x2F);

    idt_set_descriptor(32, irq_0, 0x8E);
    idt_set_descriptor(33, irq_1, 0x8E);
    idt_set_descriptor(34, irq_2, 0x8E);
    idt_set_descriptor(35, irq_3, 0x8E);
    idt_set_descriptor(36, irq_4, 0x8E);
    idt_set_descriptor(37, irq_5, 0x8E);
    idt_set_descriptor(38, irq_6, 0x8E);
    idt_set_descriptor(39, irq_7, 0x8E);
    idt_set_descriptor(40, irq_8, 0x8E);
    idt_set_descriptor(41, irq_9, 0x8E);
    idt_set_descriptor(42, irq_10, 0x8E);
    idt_set_descriptor(43, irq_11, 0x8E);
    idt_set_descriptor(44, irq_12, 0x8E);
    idt_set_descriptor(45, irq_13, 0x8E);
    idt_set_descriptor(46, irq_14, 0x8E);
    idt_set_descriptor(47, irq_15, 0x8E);

    __asm__ volatile("lidt %0\n"
                     "sti\n"
                     :
                     : "m"(idtr));
}
