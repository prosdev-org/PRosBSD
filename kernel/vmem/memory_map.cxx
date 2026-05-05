#include <libkxx/to_hex.hxx>
#include <libkxx/vector.hxx>
#include <multiboot2/memory_map.hxx>
#include <unique/log.hxx>
#include <vmem/memory_map.hxx>

namespace VMem::MemoryMap {
    struct Storage {
        kxx::Vector<Region> available;
    };

    static kxx::Vector<Region> &(*available)();

    void setup_storage() {
        static Storage storage;

        available = []() -> kxx::Vector<Region> & {
            return storage.available;
        };
    }

    kxx::StringView get_logger_prefix() {
        return "vmem/memory_map";
    }

    void init() {
        LOG("initializing");

        setup_storage();

        const auto &multiboot2_available =
                Multiboot2::MemoryMap::get_available();

        LOG("available memory:")
        for (size_t i = 0; i < multiboot2_available.get_size(); i++) {
            const PhysAddr start = {
                    .value = multiboot2_available.get(i).base_addr,
            };

            const PhysAddr end = {
                    .value = multiboot2_available.get(i).base_addr +
                             multiboot2_available.get(i).length - 1,
            };

            LOG(kxx::to_hex(start.value), " - ", kxx::to_hex(end.value))

            available().push_back({
                    .start = start,
                    .end = end,
            });
        }
    }

    const kxx::Vector<Region> &get_available() {
        return available();
    }
} // namespace VMem::MemoryMap
