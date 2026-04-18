#ifndef DEVICES_DUMMY_HXX
#define DEVICES_DUMMY_HXX

#include <devices/device.hxx>

namespace Devices {
    class Dummy final : public Device {
    public:
        explicit Dummy(
            const AutoConf::DriverHeader &driver_header,
            const AutoConf::MatchInfo &match_info) : Device(driver_header, match_info.parent) {
        }
    };
} // namespace Devices

#endif
