#include <arch/i386/cpu/gdt.hxx>
#include <unique/extern_c.h>

EXTERN_C void Arch_I386_Cpu_Gdt_init();

void Arch::I386::Cpu::Gdt::init() {
    Arch_I386_Cpu_Gdt_init();
}
