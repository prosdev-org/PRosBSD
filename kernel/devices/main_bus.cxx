#include <devices/auto_conf.hxx>
#include <devices/main_bus.hxx>

namespace Devices {
    bool MainBus::match(const AutoConf::MatchInfo &match_info) {
        return match_info.parent == nullptr;
    }

    MainBus::MainBus(
            const AutoConf::DriverHeader &device_header,
            const AutoConf::MatchInfo &match_info) : Device(device_header, match_info.parent) {
        ASSERT(match_info.parent == nullptr);

        // IsaBus
        {
            ConnectionInfo connection_info;
            connection_info.device_type = IsaBus;

            const AutoConf::MatchInfo child_match_info = {
                    .parent = this,
                    .bus_connection_info = &connection_info};

            const AutoConf::DriverHeader child_driver_header = AutoConf::match_driver(child_match_info);
            children.push_back(child_driver_header.construct(child_match_info));
        }
    }
} // namespace Devices
