#pragma once

#include <devices/auto_conf/match_info.hxx>
#include <libkxx/unique_ptr.hxx>

namespace Devices {
    class Device;
}

namespace Devices::AutoConf {
    struct MatchInfo;

    struct DriverHeader {
        const char *name;
        bool (*match)(const MatchInfo &match_info);
        kxx::UniquePtr<Device> (*construct)(const MatchInfo &match_info);
    };
} // namespace Devices::AutoConf
