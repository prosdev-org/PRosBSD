#include <sys/panic.hxx>
#include <unique/assert.h>

void ASSERT_fail(const char *statement, const char *file, int line, const char *function) {
    Sys::panic("Assertation failed");
    // TODO: use arguments
    (void) statement;
    (void) file;
    (void) line;
    (void) function;
}
