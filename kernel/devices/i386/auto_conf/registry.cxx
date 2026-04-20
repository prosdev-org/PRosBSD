#include <devices/auto_conf/registry.hxx>
#include <devices/dummy.hxx>
#include <devices/i386/i8253.hxx>
#include <devices/i386/isa_bus.hxx>
#include <devices/i386/vga_text.hxx>
#include <devices/main_bus.hxx>
#include <unique/countof.h>

namespace Devices::AutoConf::Registry {
    static DriverHeader driver_headers[] = {
            {
                    .name = "mainbus",
                    .match = [](const MatchInfo &match_info) -> bool {
                        return MainBus::match(match_info);
                    },
                    .construct = [](const MatchInfo &match_info) -> kxx::UniquePtr<Device> {
                        constexpr size_t idx = 0; // have to be the same, as the index in this array
                        return kxx::UniquePtr<Device>(new MainBus(driver_headers[idx], match_info));
                    },
            },
            {
                    .name = "isabus",
                    .match = [](const MatchInfo &match_info) -> bool {
                        return I386::IsaBus::match(match_info);
                    },
                    .construct = [](const MatchInfo &match_info) -> kxx::UniquePtr<Device> {
                        constexpr size_t idx = 1; // have to be the same, as the index in this array
                        return kxx::UniquePtr<Device>(new I386::IsaBus(driver_headers[idx], match_info));
                    },
            },
            {
                    .name = "vgatxt",
                    .match = [](const MatchInfo &match_info) -> bool {
                        return I386::VgaText::match(match_info);
                    },
                    .construct = [](const MatchInfo &match_info) -> kxx::UniquePtr<Device> {
                        constexpr size_t idx = 2; // have to be the same, as the index in this array
                        return kxx::UniquePtr<Device>(new I386::VgaText(driver_headers[idx], match_info));
                    },
            },
            {
                    .name = "i8253",
                    .match = [](const MatchInfo &match_info) -> bool {
                        return I386::I8253::match(match_info);
                    },
                    .construct = [](const MatchInfo &match_info) -> kxx::UniquePtr<Device> {
                        constexpr size_t idx = 3;
                        return kxx::UniquePtr<Device>(new I386::I8253(driver_headers[idx], match_info));
                    },
            },
            {
                    // have to be at the end
                    .name = "dummy",
                    .match = [](const MatchInfo &) -> bool {
                        return true;
                    },
                    .construct = [](const MatchInfo &match_info) -> kxx::UniquePtr<Device> {
                        constexpr size_t idx = 4;
                        return kxx::UniquePtr<Device>(new Dummy(driver_headers[idx], match_info));
                    },
            },
    };

    const DriverHeader &get_driver_header(const size_t idx) {
        ASSERT(idx < COUNTOF(driver_headers));

        return driver_headers[idx];
    }

    size_t get_driver_headers_count() {
        return COUNTOF(driver_headers);
    }
} // namespace Devices::AutoConf::Registry
