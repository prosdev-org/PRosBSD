#ifndef DEVICES_AUTO_CONF_HXX
#define DEVICES_AUTO_CONF_HXX

#include <devices/auto_conf/driver_header.hxx>
#include <devices/auto_conf/match_info.hxx>

namespace Devices::AutoConf {
    void init();
    const DriverHeader &match_driver(const MatchInfo &match_info);
} // namespace Devices::AutoConf

#endif
