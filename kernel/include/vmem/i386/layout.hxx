#ifndef VMEM_I386_LAYOUT_HXX
#define VMEM_I386_LAYOUT_HXX

#include <vmem/phys_addr.hxx>

namespace VMem::I386::Layout {
    constexpr uintptr_t start = 0x0000'0000;

    constexpr uintptr_t null_page_start = 0x0000'0000;
    constexpr uintptr_t null_page_end = 0x0000'3fff;

    // constexpr uintptr_t userland_start = 0x0000'4000;
    // constexpr uintptr_t userland_end = 0xbfff'ffff;

    constexpr uintptr_t kernel_space_start = 0xc000'0000;

    constexpr uintptr_t low_map_start = 0xc000'0000;
    constexpr uintptr_t low_map_end = 0xc03f'ffff;

    // TODO
    constexpr uintptr_t kernel_space_end = 0xc03f'ffff;

    constexpr uintptr_t end = 0xffff'ffff;
} // namespace VMem::I386::Layout

#endif
