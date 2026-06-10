#pragma once

#include <devices/auto_conf/bus_connection_info.hxx>

namespace Devices {
    class Device;
}

namespace Devices::AutoConf {
    struct MatchInfo {
        Device *parent;
        const BusConnectionInfo *bus_connection_info;
    };
} // namespace Devices::AutoConf
