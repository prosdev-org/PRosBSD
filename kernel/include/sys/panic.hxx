#ifndef SYS_PANIC_HXX
#define SYS_PANIC_HXX

#include <unique/noreturn.h>

namespace Sys {
    NORETURN void panic(const char *msg);
}

#endif
