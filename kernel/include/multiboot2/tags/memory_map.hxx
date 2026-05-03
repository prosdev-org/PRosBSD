#ifndef MULTIBOOT2_TAGS_MEMORY_MAP_HXX
#define MULTIBOOT2_TAGS_MEMORY_MAP_HXX

#include <libkxx/string.hxx>
#include <multiboot2/tags/tag_begin.hxx>

namespace Multiboot2::Tags {
    struct MemoryMap {
        struct Region {
            uint64_t base_addr;
            uint64_t length;
        };

        union RegionType {
            uint32_t value;
            enum {
                Available = 1,
                Acpi = 3,
                Hiber = 4,
                Defect = 5,
            } enum_type;

            bool operator==(const RegionType &type) const;
        };

        struct Entry {
            Region region;
            RegionType type;
            uint32_t reserved;
        };

        TagHeader tag_begin;
        uint32_t entry_size;
        uint32_t entry_version;
        Entry entries[];

        [[nodiscard]] kxx::Vector<Region> get_regions(RegionType type) const;
        [[nodiscard]] size_t get_entries_length() const;
    };

    union MemoryMapConverter {
        const TagHeader *general;
        const MemoryMap *memory_map;
    };
} // namespace Multiboot2::Tags

#endif
