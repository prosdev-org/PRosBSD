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
            I8253,
            I8042,
        };

        struct ConnectionInfo final : AutoConf::BusConnectionInfo {
            DeviceType device_type = Dummy;
        };

        union ConnectionInfoConverter {
            const AutoConf::BusConnectionInfo *general;
            const ConnectionInfo *isa_bus;
        };

    private:
        void create_child(DeviceType device_type);
    };
} // namespace Devices::I386

#endif
