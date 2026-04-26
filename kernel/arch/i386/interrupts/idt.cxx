#include <arch/i386/cpu/gdt.hxx>
#include <arch/i386/cpu/lidt.hxx>
#include <arch/i386/cpu/sti.hxx>
#include <arch/i386/interrupts/idt.hxx>
#include <arch/i386/interrupts/isr.hxx>
#include <arch/i386/interrupts/pic8259.hxx>
#include <stdint.h>
#include <unique/countof.h>

namespace Arch::I386::Interrupts::Idt {
    // If ISR addr == 0xcafebabe ->
    struct Entry {
        uint16_t isr_low; // 0xbabe
        uint16_t kernel_code_seg;
        uint8_t reserved; // always 0
        uint8_t attributes;
        uint16_t isr_high; // 0xcafe
    } __attribute__((packed));

    __attribute__((aligned(0x10))) // for performance
    static Entry idt[256];

    void set_entry(void (*isr)(), const uint8_t attributes, const uint8_t idx) {
        const auto isr_address = reinterpret_cast<uint32_t>(isr);

        Entry &entry = idt[idx];
        entry.isr_low = isr_address & 0xFFFF;
        entry.kernel_code_seg = ARCH_I386_GDT_KERN_CODE_SEG;
        entry.reserved = 0;
        entry.attributes = attributes;
        entry.isr_high = isr_address >> 16;
    }

    void init() {
        constexpr uint8_t attributes = 0x8E;

        set_entry(Isr::Arch_I386_Interrupts_Isr_0_stub, attributes, 0);
        set_entry(Isr::Arch_I386_Interrupts_Isr_1_stub, attributes, 1);
        set_entry(Isr::Arch_I386_Interrupts_Isr_2_stub, attributes, 2);
        set_entry(Isr::Arch_I386_Interrupts_Isr_3_stub, attributes, 3);
        set_entry(Isr::Arch_I386_Interrupts_Isr_4_stub, attributes, 4);
        set_entry(Isr::Arch_I386_Interrupts_Isr_5_stub, attributes, 5);
        set_entry(Isr::Arch_I386_Interrupts_Isr_6_stub, attributes, 6);
        set_entry(Isr::Arch_I386_Interrupts_Isr_7_stub, attributes, 7);
        set_entry(Isr::Arch_I386_Interrupts_Isr_8_stub, attributes, 8);
        set_entry(Isr::Arch_I386_Interrupts_Isr_9_stub, attributes, 9);
        set_entry(Isr::Arch_I386_Interrupts_Isr_10_stub, attributes, 10);
        set_entry(Isr::Arch_I386_Interrupts_Isr_11_stub, attributes, 11);
        set_entry(Isr::Arch_I386_Interrupts_Isr_12_stub, attributes, 12);
        set_entry(Isr::Arch_I386_Interrupts_Isr_13_stub, attributes, 13);
        set_entry(Isr::Arch_I386_Interrupts_Isr_14_stub, attributes, 14);
        set_entry(Isr::Arch_I386_Interrupts_Isr_15_stub, attributes, 15);
        set_entry(Isr::Arch_I386_Interrupts_Isr_16_stub, attributes, 16);
        set_entry(Isr::Arch_I386_Interrupts_Isr_17_stub, attributes, 17);
        set_entry(Isr::Arch_I386_Interrupts_Isr_18_stub, attributes, 18);
        set_entry(Isr::Arch_I386_Interrupts_Isr_19_stub, attributes, 19);
        set_entry(Isr::Arch_I386_Interrupts_Isr_20_stub, attributes, 20);
        set_entry(Isr::Arch_I386_Interrupts_Isr_21_stub, attributes, 21);

        set_entry(Isr::Arch_I386_Interrupts_Isr_Pic8259_0_stub, attributes, 32);
        set_entry(Isr::Arch_I386_Interrupts_Isr_Pic8259_1_stub, attributes, 33);
        set_entry(Isr::Arch_I386_Interrupts_Isr_Pic8259_2_stub, attributes, 34);
        set_entry(Isr::Arch_I386_Interrupts_Isr_Pic8259_3_stub, attributes, 35);
        set_entry(Isr::Arch_I386_Interrupts_Isr_Pic8259_4_stub, attributes, 36);
        set_entry(Isr::Arch_I386_Interrupts_Isr_Pic8259_5_stub, attributes, 37);
        set_entry(Isr::Arch_I386_Interrupts_Isr_Pic8259_6_stub, attributes, 38);
        set_entry(Isr::Arch_I386_Interrupts_Isr_Pic8259_7_stub, attributes, 39);

        set_entry(Isr::Arch_I386_Interrupts_Isr_Pic8259_8_stub, attributes, 40);
        set_entry(Isr::Arch_I386_Interrupts_Isr_Pic8259_9_stub, attributes, 41);
        set_entry(Isr::Arch_I386_Interrupts_Isr_Pic8259_10_stub, attributes, 42);
        set_entry(Isr::Arch_I386_Interrupts_Isr_Pic8259_11_stub, attributes, 43);
        set_entry(Isr::Arch_I386_Interrupts_Isr_Pic8259_12_stub, attributes, 44);
        set_entry(Isr::Arch_I386_Interrupts_Isr_Pic8259_13_stub, attributes, 45);
        set_entry(Isr::Arch_I386_Interrupts_Isr_Pic8259_14_stub, attributes, 46);
        set_entry(Isr::Arch_I386_Interrupts_Isr_Pic8259_15_stub, attributes, 47);

        Pic8259::init(32, 40);

        Cpu::lidt(idt, sizeof(Entry) * COUNTOF(idt) - 1);
        Cpu::sti();
    }
} // namespace Arch::I386::Interrupts::Idt
