#include <devices/device.hxx>

namespace Devices {
    Device::Device(const AutoConf::DriverHeader &driver_header, Device *parent) : driver_header(driver_header) {
        this->parent = parent;
    }
} // namespace Devices
