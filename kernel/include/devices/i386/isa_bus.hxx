#ifndef DEVICES_I386_ISA_BUS_HXX
#define DEVICES_I386_ISA_BUS_HXX

#include <devices/auto_conf/bus_connection_info.hxx>
#include <devices/device.hxx>

namespace Devices::I386 {
    class IsaBus final : public Device {
    public:
        static bool match(const AutoConf::MatchInfo &match_info);

        explicit IsaBus(
                const AutoConf::DriverHeader &driver_header,
                const AutoConf::MatchInfo &match_info);

        enum DeviceType {
            Dummy,
            VgaText,
        };

        struct ConnectionInfo final : AutoConf::BusConnectionInfo {
            DeviceType device_type = Dummy;
        };
    };
} // namespace Devices::I386

#endif
