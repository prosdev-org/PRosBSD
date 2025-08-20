#include <memory/pfa.h>
#include <memory/virtual/paging.h>
#include <stddef.h>
#include <stdint.h>

#define PAGE_DIRECTORY_SIZE 0x1000 / sizeof(uint32_t)
#define PAGE_TABLE_SIZE     0x1000 / sizeof(uint32_t)
#define PAGE_SIZE           0x1000

#define PAGING_ATTR_PRESENT    0x00000001
#define PAGING_ATTR_READ_WRITE 0x00000002

#define PAGING_ATTR_DEFAULT PAGING_ATTR_PRESENT | PAGING_ATTR_READ_WRITE
#define PAGING_ATTR_EMPTY   0x00000000

static uint32_t *page_directory;
static uint32_t *first_4mib_table;

static void *idx_to_ptr(uint32_t idx) {
    return (void *) (idx << 12);
}

void paging_init() {
    page_directory = idx_to_ptr(pf_alloc());
    for (size_t i = 0; i < PAGE_DIRECTORY_SIZE; i++) {
        page_directory[i] = PAGING_ATTR_EMPTY;
    }

    // Page first 4MiB to high addr
    first_4mib_table = idx_to_ptr(pf_alloc());
    for (size_t i = 0; i < PAGE_DIRECTORY_SIZE; i++) {
        first_4mib_table[i] = i << 12 | PAGING_ATTR_DEFAULT;
    }

    page_directory[(PAGING_FIRST_4MIB_MAPPING_ADDR >> 20) / sizeof(uint32_t)] =
            ((uint32_t) first_4mib_table) | PAGING_ATTR_DEFAULT;

    __asm__ volatile("mov %0, %%cr3\n" : : "r"(page_directory));
}
