#ifndef SYS_TIMER_HXX
#define SYS_TIMER_HXX

#include <sys/time.hxx>

namespace Sys {
    class Timer {
    public:
        virtual ~Timer() = default;
        virtual Time::Duration get_since_start() = 0;
    };
} // namespace Sys

#endif
