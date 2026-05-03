#include <libkxx/math.hxx>
#include <machine/paging.hxx>
#include <multiboot2/memory_map.hxx>
#include <unique/extern_c.h>
#include <vmem/i386/layout.hxx>

namespace Multiboot2::MemoryMap {
    struct Storage {
        kxx::Vector<Tags::MemoryMap::Region> available;
    };

    static kxx::Vector<Tags::MemoryMap::Region> &(*available)();

    void setup_storage(
            kxx::Vector<Tags::MemoryMap::Region> &&new_available) {
        static Storage storage = {
                .available = kxx::move(new_available),
        };

        available = []() -> kxx::Vector<Tags::MemoryMap::Region> & {
            return storage.available;
        };
    }

    void sanitize_available() {
        kxx::Vector<Tags::MemoryMap::Region> new_available;

        for (size_t i = 0; i < available().get_size(); i++) {
            const Tags::MemoryMap::Region region = available().get(i);

            uint64_t start = region.base_addr;
            uint64_t end = region.base_addr + region.length - 1;

            start = kxx::Math::max(
                    start,
                    VMem::PhysAddr::from_virt(get_kernel_end()).value);

            end = kxx::Math::min(
                    end,
                    static_cast<uint64_t>(0xffff'ffff));

            // align by Machine::Paging::get_page_size()
            if (start % Machine::Paging::get_page_size() != 0) {
                start = (start / Machine::Paging::get_page_size() + 1) *
                        Machine::Paging::get_page_size();
            }

            // align by Machine::Paging::get_page_size()
            end -= end % Machine::Paging::get_page_size();

            if (start <= end) {
                new_available.push_back({
                        .base_addr = start,
                        .length = end - start + 1,
                });
            }
        }

        available() = new_available;
    }

    void init(kxx::Vector<Tags::MemoryMap::Region> &&new_available) {
        setup_storage(kxx::move(new_available));
        sanitize_available();
    }

    const kxx::Vector<Tags::MemoryMap::Region> &get_available() {
        return available();
    }

    EXTERN_C int __bootstrap_heap_start;
    EXTERN_C int __bootstrap_heap_end;
    EXTERN_C int __kernel_end;

    uintptr_t get_bootstrap_heap_start() {
        return reinterpret_cast<uintptr_t>(&__bootstrap_heap_start);
    }

    uintptr_t get_bootstrap_heap_end() {
        return reinterpret_cast<uintptr_t>(&__bootstrap_heap_end);
    }

    uintptr_t get_kernel_end() {
        return reinterpret_cast<uintptr_t>(&__kernel_end);
    }
} // namespace Multiboot2::MemoryMap
