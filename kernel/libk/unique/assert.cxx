#include <sys/panic.hxx>
#include <unique/assert.h>

void ASSERT_fail(const char *statement, const char *file, const int line, const char *function) {
    (void) line;
    Sys::panic("Assertation failed: ", statement, " (", file, ": ", function, ")");
}
