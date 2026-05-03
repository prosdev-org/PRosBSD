#include <libkxx/bitmap.hxx>
#include <machine/paging.hxx>
#include <sys/panic.hxx>
#include <vmem/memory_map.hxx>
#include <vmem/page_frame_allocator.hxx>

namespace VMem::PageFrameAllocator {
    struct Storage {
        kxx::Bitmap bitmap; // false - free, true - occupied
    };

    static kxx::Bitmap &(*bitmap)();

    static size_t last_alloc = 0;

    void setup_storage() {
        static Storage storage = {
                .bitmap = kxx::Bitmap(Machine::Paging::get_total_pages()),
        };

        bitmap = []() -> kxx::Bitmap & {
            return storage.bitmap;
        };
    }

    size_t phys_addr_to_idx(const PhysAddr phys_addr) {
        ASSERT(phys_addr.value <= 0xffff'ffff);
        return phys_addr.value / Machine::Paging::get_page_size();
    }

    void init() {
        setup_storage();

        bitmap().set_all(true);

        const auto &available = MemoryMap::get_available();

        for (size_t i = 0; i < available.get_size(); i++) {
            bitmap().set_range(
                    false,
                    phys_addr_to_idx(available.get(i).start),
                    phys_addr_to_idx(available.get(i).end));
        }
    }

    PageFrame alloc() {
        kxx::Bitmap &local_bitmap = bitmap(); // for performance

        for (size_t i = last_alloc; i < local_bitmap.get_size(); i++) {
            if (!local_bitmap.get(i)) {
                local_bitmap.set(true, i);
                last_alloc = i;
                return PageFrame(i);
            }
        }

        for (size_t i = 0; i < last_alloc; i++) {
            if (!local_bitmap.get(i)) {
                local_bitmap.set(true, i);
                last_alloc = i;
                return PageFrame(i);
            }
        }

        Sys::panic("PageFrameAllocator: out of page frames");
    }

    void free(const PageFrame page_frame) {
        ASSERT(bitmap().get(page_frame.get_idx()));

        bitmap().set(false, page_frame.get_idx());
    }
} // namespace VMem::PageFrameAllocator
