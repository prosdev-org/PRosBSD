#ifndef SYS_DUMMY_TIMER_HXX
#define SYS_DUMMY_TIMER_HXX

#include <sys/timer.hxx>

namespace Sys {
    class DummyTimer final : public Timer {
    public:
        Time::Duration get_since_start() override;
    };
} // namespace Sys

#endif
