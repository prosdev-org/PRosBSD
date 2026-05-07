#include <arch/i386/cpu/invlpg.hxx>
#include <arch/i386/cpu/load_cr3.hxx>
#include <machine/paging.hxx>
#include <unique/log.hxx>
#include <vmem/i386/layout.hxx>
#include <vmem/memory_map.hxx>
#include <vmem/page_frame_allocator.hxx>
#include <vmem/paging.hxx>

namespace VMem::Paging {
    constexpr uint32_t attribute_empty = 0x0000'0000;
    constexpr uint32_t attribute_present = 0x0000'0001;
    constexpr uint32_t attribute_read_write = 0x0000'0002;
    // constexpr uint32_t attribute_user = 0x0000'0004;
    constexpr uint32_t attribute_default =
            attribute_present | attribute_read_write;

    constexpr size_t page_dir_recursive_idx = 0x3ff;

    static bool initialized = false;

    kxx::StringView get_logger_prefix() {
        return "vmem/paging";
    }

    void calc_idxes(const uintptr_t virt_addr,
                    size_t &page_dir_idx, size_t &page_table_idx) {
        page_dir_idx = virt_addr >> 22;
        page_table_idx = (virt_addr >> 12) & 0x3ff;
    }

    uint32_t *get_page_dir_entry(const size_t page_dir_idx) {
        ASSERT(page_dir_idx <
               (Machine::Paging::get_page_size() / sizeof(uint32_t)));

        const uintptr_t page_dir_base =
                I386::Layout::page_mapping_start +
                page_dir_recursive_idx * Machine::Paging::get_page_size();

        return reinterpret_cast<uint32_t *>(
                page_dir_base + page_dir_idx * sizeof(uint32_t));
    }

    uint32_t *get_page_table(const size_t page_dir_idx) {
        ASSERT(page_dir_idx <
               (Machine::Paging::get_page_size() / sizeof(uint32_t)));

        return reinterpret_cast<uint32_t *>(
                I386::Layout::page_mapping_start +
                page_dir_idx * Machine::Paging::get_page_size());
    }

    uint32_t *get_page_table_entry(
            const size_t page_dir_idx, const size_t page_table_idx) {
        ASSERT(page_dir_idx <
               (Machine::Paging::get_page_size() / sizeof(uint32_t)));
        ASSERT(page_table_idx <
               (Machine::Paging::get_page_size() / sizeof(uint32_t)));

        return reinterpret_cast<uint32_t *>(
                I386::Layout::page_mapping_start +
                page_dir_idx * Machine::Paging::get_page_size() +
                page_table_idx * sizeof(uint32_t));
    }

    bool is_page_dir_entry_present(const size_t page_dir_idx) {
        const uint32_t *page_dir_entry = get_page_dir_entry(page_dir_idx);
        return *page_dir_entry & attribute_present;
    }

    bool is_page_table_entry_present(
            const size_t page_dir_idx, const size_t page_table_idx) {
        const uint32_t *page_table_entry =
                get_page_table_entry(page_dir_idx, page_table_idx);
        return *page_table_entry & attribute_present;
    }

    void ensure_page_table_exists(const size_t page_dir_idx) {
        if (is_page_dir_entry_present(page_dir_idx)) {
            return;
        }

        uint32_t *page_dir_entry = get_page_dir_entry(page_dir_idx);
        const PageFrame page_table_frame = PageFrameAllocator::alloc();

        *page_dir_entry =
                page_table_frame.get_phys_addr().value | attribute_default;

        uint32_t *page_table = get_page_table(page_dir_idx);
        for (size_t i = 0;
             i < Machine::Paging::get_page_size() / sizeof(uint32_t);
             i++) {
            page_table[i] = attribute_empty;
        }
    }

    size_t get_page_table_present_entries_count(const size_t page_dir_idx) {
        if (!is_page_dir_entry_present(page_dir_idx)) {
            return 0;
        }

        size_t count = 0;

        for (size_t i = 0;
             i < Machine::Paging::get_page_size() / sizeof(uint32_t);
             i++) {
            if (is_page_table_entry_present(page_dir_idx, i)) {
                count++;
            }
        }

        return count;
    }

    /*
     * returns nullptr, if virt_addr is not mapped
     */
    kxx::UniquePtr<PhysAddr> get_physical_address(const uintptr_t virt_addr) {
        ASSERT(virt_addr % Machine::Paging::get_page_size() == 0);

        size_t page_dir_idx;
        size_t page_table_idx;
        calc_idxes(virt_addr, page_dir_idx, page_table_idx);

        if (!is_page_dir_entry_present(page_dir_idx) ||
            !is_page_table_entry_present(page_dir_idx, page_table_idx)) {
            return kxx::UniquePtr<PhysAddr>(nullptr);
        }

        const uint32_t *page_table_entry =
                get_page_table_entry(page_dir_idx, page_table_idx);

        auto phys_addr = kxx::UniquePtr(new PhysAddr{
                .value = *page_table_entry & 0xffff'f000,
        });

        return phys_addr;
    }

    void init() {
        LOG("initializing");

        const PageFrame page_dir_frame = PageFrameAllocator::alloc();
        const auto page_dir = reinterpret_cast<uint32_t *>(
                page_dir_frame.get_phys_addr().to_virt());

        for (size_t i = 0;
             i < Machine::Paging::get_page_size() / sizeof(uint32_t);
             i++) {
            page_dir[i] = attribute_empty;
        }

        page_dir[page_dir_recursive_idx] =
                static_cast<uint32_t>(page_dir_frame.get_phys_addr().value) |
                attribute_default;

        constexpr PhysAddr low_map_end_phys = {
                .value = I386::Layout::page_mapping_end -
                         I386::Layout::page_mapping_start,
        };

        // Address space which one page_table can handle
        ASSERT(low_map_end_phys.value == 0x003f'ffff);

        const PageFrame low_page_table_frame = PageFrameAllocator::alloc();
        const auto low_page_table = reinterpret_cast<uint32_t *>(
                low_page_table_frame.get_phys_addr().to_virt());

        for (size_t i = 0;
             i < Machine::Paging::get_page_size() / sizeof(uint32_t);
             i++) {
            low_page_table[i] = (i << 12) | attribute_default;
        }

        page_dir[I386::Layout::low_map_start >> 22] =
                low_page_table_frame.get_phys_addr().value | attribute_default;

        Arch::I386::Cpu::load_cr3(page_dir_frame.get_phys_addr().value);

        initialized = true;
    }

    void map(const PhysAddr phys_addr, const uintptr_t virt_addr) {
        ASSERT(initialized);
        ASSERT(phys_addr.value <= 0xffff'ffff);
        ASSERT(phys_addr.value % Machine::Paging::get_page_size() == 0);
        ASSERT(virt_addr % Machine::Paging::get_page_size() == 0);

        size_t page_dir_idx;
        size_t page_table_idx;
        calc_idxes(virt_addr, page_dir_idx, page_table_idx);

        ensure_page_table_exists(page_dir_idx);

        uint32_t *page_table_entry =
                get_page_table_entry(page_dir_idx, page_table_idx);

        *page_table_entry =
                static_cast<uint32_t>(phys_addr.value) | attribute_default;

        Arch::I386::Cpu::invlpg(virt_addr);
    }

    void unmap(const uintptr_t virt_addr) {
        ASSERT(initialized);
        ASSERT(virt_addr % Machine::Paging::get_page_size() == 0);

        size_t page_dir_idx;
        size_t page_table_idx;
        calc_idxes(virt_addr, page_dir_idx, page_table_idx);

        ASSERT(is_page_table_entry_present(page_dir_idx, page_table_idx));

        uint32_t *page_table_entry =
                get_page_table_entry(page_dir_idx, page_table_idx);

        *page_table_entry &= ~attribute_present;

        if (get_page_table_present_entries_count(page_dir_idx) == 0) {
            const auto page_table =
                    reinterpret_cast<uintptr_t>(get_page_table(page_dir_idx));

            const kxx::UniquePtr<PhysAddr> phys_addr =
                    get_physical_address(page_table);

            ASSERT(!phys_addr.is_nullptr());

            PageFrameAllocator::free(PageFrame(*phys_addr));
        }
    }
} // namespace VMem::Paging
