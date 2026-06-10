#pragma once

#include <devices/auto_conf/driver_header.hxx>

namespace Devices::AutoConf::Registry {
    const DriverHeader &get_driver_header(size_t idx);
    size_t get_driver_headers_count();
} // namespace Devices::AutoConf::Registry
