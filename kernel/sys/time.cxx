#include <sys/time.hxx>

namespace Sys::Time {
    Duration::Duration(const unsigned long long nanos) : nanos(nanos) {
    }

    unsigned long long Duration::as_nanos() const {
        return nanos;
    }

    unsigned long long Duration::as_micros() const {
        return nanos / 1000ULL;
    }

    unsigned long long Duration::as_millis() const {
        return nanos / 1000000ULL;
    }

    unsigned long long Duration::as_seconds() const {
        return nanos / 1000000000ULL;
    }
} // namespace Sys::Time
