#ifndef MULTIBOOT2_TAGS_BOOTLOADER_NAME_HXX
#define MULTIBOOT2_TAGS_BOOTLOADER_NAME_HXX

#include <libkxx/string.hxx>
#include <multiboot2/tags/tag_begin.hxx>

namespace Multiboot2::Tags {
    struct BootloaderName {
        TagHeader tag_begin;
        const char string[];

        [[nodiscard]] kxx::String get_string() const;
    };

    union BootloaderNameConverter {
        const TagHeader *general;
        const BootloaderName *bootloader_name;
    };
} // namespace Multiboot2::Tags

#endif
