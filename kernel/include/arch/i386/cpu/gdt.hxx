#ifndef ARCH_I386_GDT_HXX
#define ARCH_I386_GDT_HXX

#define ARCH_I386_GDT_KERN_CODE_SEG 0x08
#define ARCH_I386_GDT_KERN_DATA_SEG 0x10
#define ARCH_I386_GDT_USER_CODE_SEG (0x18 | 3)
#define ARCH_I386_GDT_USER_DATA_SEG (0x20 | 3)
#define ARCH_I386_GDT_TSS_SEG       0x28

#ifndef __ASSEMBLER__

namespace Arch::I386::Cpu::Gdt {
    void init();
} // namespace Arch::I386::Cpu::Gdt

#endif

#endif
