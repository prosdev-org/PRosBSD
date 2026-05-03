#include <multiboot2/tags/memory_map.hxx>
#include <unique/assert.h>

namespace Multiboot2::Tags {
    bool MemoryMap::RegionType::operator==(const RegionType &type) const {
        return this->value == type.value;
    }

    kxx::Vector<MemoryMap::Region> MemoryMap::get_regions(const RegionType type) const {
        kxx::Vector<Region> result;
        for (size_t i = 0; i < get_entries_length(); i++) {
            if (entries[i].type == type) {
                result.push_back(entries[i].region);
            }
        }

        return result;
    }

    size_t MemoryMap::get_entries_length() const {
        ASSERT(entry_size == sizeof(Entry));

        return (tag_begin.size - sizeof(tag_begin)) / sizeof(Entry);
    }
} // namespace Multiboot2::Tags
