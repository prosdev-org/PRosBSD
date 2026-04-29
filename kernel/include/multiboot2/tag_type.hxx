#ifndef MULTIBOOT2_TAG_TYPE_HXX
#define MULTIBOOT2_TAG_TYPE_HXX

#include <stdint.h>

namespace Multiboot2 {
    union TagType {
        uint32_t value;
        enum {
            BasicMemoryInfo = 4,
            BiosBootDevice = 5,
            BootCmdLine = 1,
            Modules = 3,
            ElfSymbols = 9,
            MemoryMap = 6,
            BootloaderName = 2,
            ApmTable = 10,
            VbeInfo = 7,
            FramebufferInfo = 8,
            Efi32SysTablePtr = 11,
            Efi64SysTablePtr = 12,
            SMBiosTables = 13,
            AcpiOldRsdp = 14,
            AcpiNewRsdp = 15,
            NetworkingInfo = 16,
            EfiMemoryMap = 17,
            EfiBootServNotTerminated = 18,
            Efi32ImageHandlePtr = 19,
            Efi64ImageHandlePtr = 20,
            ImageLoadBasePhysAddr = 21,
        };
    };
} // namespace Multiboot2

#endif
