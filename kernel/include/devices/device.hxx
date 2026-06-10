#pragma once

#include <devices/auto_conf/driver_header.hxx>
#include <libkxx/unique_ptr.hxx>
#include <libkxx/vector.hxx>

namespace Devices {
    namespace AutoConf {
        struct DriverHeader;
    }

    class Device {
    public:
        Device() = delete;
        explicit Device(Device *parent) = delete;
        explicit Device(const AutoConf::DriverHeader &driver_header, Device *parent);
        virtual ~Device() = default;
        const AutoConf::DriverHeader &driver_header;
        Device *parent = nullptr;
        kxx::Vector<kxx::UniquePtr<Device>> children; // empty for non-buses
    };
} // namespace Devices
