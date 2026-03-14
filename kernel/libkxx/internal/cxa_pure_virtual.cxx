#include <sys/panic.hxx>
#include <unique/extern_c.h>

// NOLINTNEXTLINE(bugprone-reserved-identifier)
EXTERN_C void __cxa_pure_virtual() {
    Sys::panic("Attempted to call pure virtual function");
}
