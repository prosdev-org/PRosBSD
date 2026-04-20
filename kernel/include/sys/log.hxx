#ifndef SYS_LOG_HXX
#define SYS_LOG_HXX

#include <libkxx/print.hxx>
#include <sys/kernel.hxx>

namespace Sys {
    template<typename... Args>
    void log(Args... args) {
        kxx::println(Kernel::get_timer().get_since_start(), ' ', args...);
    }
} // namespace Sys

#endif
