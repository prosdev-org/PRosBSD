#ifndef MULTIBOOT2_TAGS_TAG_BEGIN_HXX
#define MULTIBOOT2_TAGS_TAG_BEGIN_HXX

#include <multiboot2/tag_type.hxx>
#include <stdint.h>

namespace Multiboot2::Tags {
    struct TagHeader {
        TagType type;
        static_assert(sizeof(type) == sizeof(uint32_t));

        uint32_t size;
    };
} // namespace Multiboot2::Tags

#endif
