#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

static bool panicked = false;

struct stackframe {
    struct stackframe *ebp;
    uint32_t eip;
};

void panic(const char *s) {
    uint32_t ebp, esp;
    __asm__ volatile("mov %%ebp, %0\n"
                     "mov %%esp, %1\n"
                     : "=r"(ebp), "=r"(esp));

    uint32_t eax, ebx, ecx, edx, esi, edi;
    __asm__ volatile("mov %%eax, %0\n"
                     "mov %%ebx, %1\n"
                     "mov %%ecx, %2\n"
                     "mov %%edx, %3\n"
                     "mov %%esi, %4\n"
                     "mov %%edi, %5\n"
                     : "=r"(eax), "=r"(ebx), "=r"(ecx), "=r"(edx), "=r"(esi), "=r"(edi));

    uint32_t cr0, cr2, cr3, cr4;
    __asm__ volatile("mov %%cr0, %0\n"
                     "mov %%cr2, %1\n"
                     "mov %%cr3, %2\n"
                     "mov %%cr4, %3\n"
                     : "=r"(cr0), "=r"(cr2), "=r"(cr3), "=r"(cr4));

    if (panicked)
        printf("\n\033[37m\033[41mNested panic: ");
    else
        printf("\n\033[37m\033[44mKernel panic: ");
    printf("%s\n"
           "EBP: 0x%x, ESP: 0x%x\n"
           "Registers: EAX: 0x%x, EBX: 0x%x, ECX: 0x%x,\nEDX: 0x%x ESI: 0x%x, EDI: 0x%x\n"
           "Control Registers: CR0: 0x%x, CR2: 0x%x, CR3: 0x%x, CR4: 0x%x\n",
           s, ebp, esp, eax, ebx, ecx, edx, esi, edi, cr0, cr2, cr3, cr4);

    if (panicked)
        goto halt;
    panicked = true;

    struct stackframe *stk;
    __asm__ volatile("movl %%ebp, %0" : "=r"(stk)::);

    printf("Stack trace:\n");
    for (uint32_t frame = 0; stk && frame < 10; ++frame) {
        printf("  0x%x\n", stk->eip);
        stk = stk->ebp;
    }

halt:
    __asm__ volatile("cli\n"
                     "hlt\n"
                     :
                     :);
}
