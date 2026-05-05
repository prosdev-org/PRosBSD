#include <arch/i386/port_io.hxx>
#include <devices/i386/i8253.hxx>
#include <devices/i386/isa_bus.hxx>
#include <libkxx/string_view.hxx>
#include <machine/interrupts.hxx>
#include <sys/kernel.hxx>
#include <unique/log.hxx>

namespace Devices::I386 {
    class I8253ToTimerAdapter final : public Sys::Timer {
    public:
        explicit I8253ToTimerAdapter(I8253 *i8253);
        Sys::Time::Duration get_since_start() override;

    private:
        I8253 *i8253;
    };

    static kxx::StringView get_logger_prefix() {
        return "devices/i386/i8253";
    }

    bool I8253::match(const AutoConf::MatchInfo &match_info) {
        ASSERT(match_info.parent != nullptr);

        if (kxx::StringView(match_info.parent->driver_header.name) != "isabus") {
            return false;
        }

        const IsaBus::ConnectionInfoConverter converter = {
                .general = match_info.bus_connection_info,
        };

        return converter.isa_bus->device_type == IsaBus::I8253;
    }

    I8253::I8253(
            const AutoConf::DriverHeader &driver_header,
            const AutoConf::MatchInfo &match_info) : Device(driver_header, match_info.parent) {
        constexpr uint8_t command_port = 0x43;
        constexpr uint8_t channel0_data_port = 0x40;

        LOG("initializing");

        Arch::I386::PortIO::out(0x36, command_port);
        Arch::I386::PortIO::out(divisor & 0xff, channel0_data_port);
        Arch::I386::PortIO::out(divisor >> 8, channel0_data_port);

        LOG("target_freq: ", target_freq);
        LOG("divisor: ", divisor);

        constexpr uint8_t interrupt_idx = 0;
        Machine::Interrupts::bind_handler(this, interrupt_idx);

        Sys::Kernel::set_timer(as_timer());
    }

    void I8253::handle() {
        ticks++;
    }

    kxx::UniquePtr<Sys::Timer> I8253::as_timer() {
        return kxx::UniquePtr(static_cast<Sys::Timer *>(new I8253ToTimerAdapter(this)));
    }

    I8253ToTimerAdapter::I8253ToTimerAdapter(I8253 *i8253) {
        this->i8253 = i8253;
    }

    Sys::Time::Duration I8253ToTimerAdapter::get_since_start() {
        return Sys::Time::Duration(i8253->ticks * (1000000000ULL * I8253::divisor / I8253::base_freq));
    }
} // namespace Devices::I386
