#include <libkxx/print.hxx>
#include <multiboot2/memory_map.hxx>
#include <multiboot2/tags/bootloader_name.hxx>
#include <multiboot2/tags/memory_map.hxx>
#include <multiboot2/tags/tag_begin.hxx>
#include <unique/extern_c.h>

namespace Multiboot2 {
    EXTERN_C uintptr_t multiboot2_boot_info_phys;
    uintptr_t multiboot2_boot_info_phys = 0;

    constexpr uintptr_t tag_alignment = 8;
    uint32_t total_tags_size;

    Tags::TagHeader *next_tag() {
        static uint32_t next_tag_addr = multiboot2_boot_info_phys + tag_alignment;

        if (next_tag_addr >= multiboot2_boot_info_phys + total_tags_size) {
            return nullptr;
        }

        const auto tag_begin = reinterpret_cast<Tags::TagHeader *>(next_tag_addr);

        next_tag_addr += tag_begin->size;
        if (next_tag_addr % tag_alignment != 0) {
            next_tag_addr += tag_alignment - (next_tag_addr % tag_alignment);
        }

        return tag_begin;
    }

    void handle_tag(const Tags::TagHeader *tag_begin) {
        switch (tag_begin->type.value) {
            // Demo
            case TagType::BootloaderName: {
                const Tags::BootloaderNameConverter converter = {
                        .general = tag_begin,
                };

                kxx::println(
                        "Bootloader name: ",
                        converter.bootloader_name->get_string());
            } break;
            case TagType::MemoryMap: {
                const Tags::MemoryMapConverter converter = {
                        .general = tag_begin,
                };

                auto available =
                        converter.memory_map->get_regions({
                                .enum_type =
                                        Tags::MemoryMap::RegionType::Available,
                        });

                MemoryMap::init(kxx::move(available));

                uint64_t total_length = 0;
                for (size_t i = 0; i < MemoryMap::get_available().get_size(); i++) {
                    total_length += MemoryMap::get_available().get(i).length;
                }

                kxx::println("Total memory: ", total_length / 1024 / 1024, "MiB");

            } break;
            default:;
        }
    }

    void init() {
        struct TagsHeader {
            uint32_t total_size;
            uint32_t reserved;
        };

        const auto tags = reinterpret_cast<TagsHeader *>(
                multiboot2_boot_info_phys);
        total_tags_size = tags->total_size;

        const Tags::TagHeader *tag_begin = next_tag();
        while (tag_begin != nullptr) {
            handle_tag(tag_begin);
            tag_begin = next_tag();
        }
    }
} // namespace Multiboot2
