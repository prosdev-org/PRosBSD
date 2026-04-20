#ifndef DEVICES_I386_I8253_HXX
#define DEVICES_I386_I8253_HXX

#include <devices/device.hxx>
#include <machine/interrupts.hxx>
#include <sys/timer.hxx>

namespace Devices::I386 {
    class I8253 final : public Device, public Machine::Interrupts::Handler {
        friend class I8253ToTimerAdapter;

    public:
        static bool match(const AutoConf::MatchInfo &match_info);

        explicit I8253(
                const AutoConf::DriverHeader &driver_header,
                const AutoConf::MatchInfo &match_info);
        void handle() override;
        kxx::UniquePtr<Sys::Timer> as_timer();

    private:
        static constexpr uint32_t base_freq = 1193180;
        static constexpr uint32_t target_freq = 1000;
        static constexpr uint16_t divisor = base_freq / target_freq;

        unsigned long long ticks = 0;
    };
} // namespace Devices::I386

#endif
