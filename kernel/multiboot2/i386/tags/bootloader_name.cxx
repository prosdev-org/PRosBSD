#include <multiboot2/tags/bootloader_name.hxx>

namespace Multiboot2::Tags {
    kxx::String BootloaderName::get_string() const {
        return kxx::String(string, tag_begin.size - sizeof(tag_begin));
    }
} // namespace Multiboot2::Tags
