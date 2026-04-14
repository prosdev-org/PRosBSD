#include <devices/auto_conf.hxx>
#include <devices/auto_conf/registry.hxx>
#include <devices/device.hxx>
#include <libkxx/print.hxx>
#include <libkxx/unique_ptr.hxx>

namespace Devices::AutoConf {
    struct Storage {
        kxx::UniquePtr<Device> main_bus;
    };

    static kxx::UniquePtr<Device> &(*main_bus)();

    void setup_storage() {
        static Storage storage = {
                .main_bus = kxx::UniquePtr<Device>(nullptr),
        };

        main_bus = []() -> kxx::UniquePtr<Device> & {
            return storage.main_bus;
        };
    }

    static bool initialized = false;

    void init() {
        setup_storage();

        constexpr MatchInfo match_info = {
                nullptr, nullptr};
        main_bus() = match_driver(match_info).construct(match_info);
        initialized = true;
    }

    const DriverHeader &match_driver(const MatchInfo &match_info) {
        for (size_t i = 0; i < Registry::get_driver_headers_count(); i++) {
            if (Registry::get_driver_header(i).match(match_info)) {
                return Registry::get_driver_header(i);
            }
        }

        ASSERT(false);
    }
} // namespace Devices::AutoConf
