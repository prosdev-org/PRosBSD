#ifndef DEVICES_MAIN_BUS_HXX
#define DEVICES_MAIN_BUS_HXX

#include <devices/auto_conf/match_info.hxx>
#include <devices/device.hxx>

namespace Devices {
    class MainBus final : public Device {
    public:
        static bool match(const AutoConf::MatchInfo &match_info);

        MainBus(
                const AutoConf::DriverHeader &device_header,
                const AutoConf::MatchInfo &match_info);

        enum DeviceType {
            Dummy,
            IsaBus
        };

        struct ConnectionInfo final : AutoConf::BusConnectionInfo {
            DeviceType device_type = Dummy;
        };
    };
} // namespace Devices

#endif
