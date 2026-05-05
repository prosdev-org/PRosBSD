#include <devices/auto_conf.hxx>
#include <devices/i386/isa_bus.hxx>
#include <devices/main_bus.hxx>
#include <libkxx/string_view.hxx>
#include <unique/log.hxx>

namespace Devices::I386 {
    static kxx::StringView get_logger_prefix() {
        return "devices/i386/isa_bus";
    }

    bool IsaBus::match(const AutoConf::MatchInfo &match_info) {
        ASSERT(match_info.parent != nullptr);

        if (kxx::StringView(match_info.parent->driver_header.name) != "mainbus") {
            return false;
        }

        const auto *bus_connection_info = (MainBus::ConnectionInfo *) match_info.bus_connection_info;
        return bus_connection_info->device_type == MainBus::IsaBus;
    }

    IsaBus::IsaBus(
            const AutoConf::DriverHeader &driver_header,
            const AutoConf::MatchInfo &match_info) : Device(driver_header, match_info.parent) {
        LOG("initializing");

        ASSERT(parent != nullptr);

        create_child(VgaText);
        create_child(I8253);
        create_child(I8042);
    }

    void IsaBus::create_child(const DeviceType device_type) {
        LOG("creating child: type: ", device_type);

        ConnectionInfo connection_info;
        connection_info.device_type = device_type;

        const AutoConf::MatchInfo child_match_info = {
                .parent = this,
                .bus_connection_info = &connection_info};

        const AutoConf::DriverHeader child_driver_header = AutoConf::match_driver(child_match_info);
        children.push_back(child_driver_header.construct(child_match_info));
    }
} // namespace Devices::I386
