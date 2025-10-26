#include <core/panic.h>
#include <memory/pfa.h>
#include <memory/virtual/layout.h>
#include <memory/virtual/paging.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define PAGE_DIRECTORY_SIZE 0x1000
#define PAGE_TABLE_SIZE     0x1000

#define PAGING_ATTR_PRESENT    0x00000001
#define PAGING_ATTR_READ_WRITE 0x00000002

#define PAGING_ATTR_DEFAULT (PAGING_ATTR_PRESENT | PAGING_ATTR_READ_WRITE)
#define PAGING_ATTR_EMPTY   0x00000000

// Recursive paging
#define PAGE_TABLES_BASE    MEM_VIRT_LAYOUT_PAGE_MAPPING_START
#define PAGE_DIRECTORY_IDX  0x3FF
#define PAGE_DIRECTORY_BASE (PAGE_TABLES_BASE + PAGE_DIRECTORY_IDX * PAGE_TABLE_SIZE)

static void load_cr3(const uint32_t page_directory_addr) {
    __asm__ volatile("mov %0, %%cr3" : : "r"(page_directory_addr));
}

static void invlpg(const uint32_t addr) {
    __asm__ volatile("invlpg %0" : : "m"(addr));
}

static void calc_idxes(const uintptr_t addr, size_t *pd_idx, size_t *pt_idx) {
    const size_t idx = (size_t) addr >> 0xC;
    *pd_idx = idx >> 0xA;
    *pt_idx = idx & 0x3FF;
}

static void *ptr_for_pde(const size_t pd_idx) {
    return (void *) (PAGE_DIRECTORY_BASE + pd_idx * sizeof(uint32_t));
}

static void *ptr_for_pt(const size_t pd_idx) {
    return (void *) (PAGE_TABLES_BASE + pd_idx * PAGE_TABLE_SIZE);
}

static void *ptr_for_pte(const size_t pd_idx, const size_t pt_idx) {
    return (void *) (PAGE_TABLES_BASE + pd_idx * PAGE_TABLE_SIZE + pt_idx * sizeof(uint32_t));
}

static void ensure_pt(const size_t pd_idx) {
    uint32_t *pde = ptr_for_pde(pd_idx);
    if (*pde & PAGING_ATTR_PRESENT) {
        return;
    }

    const uintptr_t pt_phys = pf_alloc();
    *pde = pt_phys | PAGING_ATTR_DEFAULT;

    void *pt = ptr_for_pt(pd_idx);
    memset(pt, PAGING_ATTR_EMPTY, PAGE_TABLE_SIZE);
}

static void map_page(uintptr_t phys, uintptr_t virt) {
    phys &= 0xFFFFF000;
    virt &= 0xFFFFF000;

    size_t pd_idx;
    size_t pt_idx;
    calc_idxes(virt, &pd_idx, &pt_idx);

    ensure_pt(pd_idx);

    uint32_t *pte = ptr_for_pte(pd_idx, pt_idx);
    *pte = phys | PAGING_ATTR_DEFAULT;
    invlpg(virt);
}

void *paging_addr_phys_to_virt(const uintptr_t phys) {
    if (phys > MEM_VIRT_LAYOUT_KERNEL_LOW_END - MEM_VIRT_LAYOUT_KERNEL_LOW_START) {
        panic("Paging: tried to convert the physical address to virtual, which is out of the low 4 MiB range");
    }

    return (void *) (phys + MEM_VIRT_LAYOUT_KERNEL_LOW_START);
}

void paging_init() {
    const uintptr_t pd_phys = pf_alloc();
    uint32_t *pd = paging_addr_phys_to_virt(pd_phys);

    memset(pd, PAGING_ATTR_EMPTY, PAGE_DIRECTORY_SIZE);
    pd[PAGE_DIRECTORY_IDX] = pd_phys | PAGING_ATTR_DEFAULT;

    const uintptr_t low_pt_phys = pf_alloc();
    uint32_t *low_pt = paging_addr_phys_to_virt(low_pt_phys);

    for (unsigned i = 0; i < PAGE_TABLE_SIZE / sizeof(uint32_t); i++) {
        low_pt[i] = (i << 0xC) | PAGING_ATTR_DEFAULT;
    }
    pd[(MEM_VIRT_LAYOUT_KERNEL_LOW_START >> 0x16)] = low_pt_phys | PAGING_ATTR_DEFAULT;

    load_cr3(pd_phys);
}
