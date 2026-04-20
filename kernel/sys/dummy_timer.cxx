#include <sys/dummy_timer.hxx>

namespace Sys {
    Time::Duration DummyTimer::get_since_start() {
        return Time::Duration(0ULL);
    }
} // namespace Sys
