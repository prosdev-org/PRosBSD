#ifndef LIBK_UNIQUE_ASSERT_H
#define LIBK_UNIQUE_ASSERT_H

#include <unique/extern_c.h>
#include <unique/noreturn.h>

#define ASSERT(x) ((void) ((x) || (ASSERT_fail(#x, __FILE__, __LINE__, __func__), 0)))

EXTERN_C NORETURN void ASSERT_fail(const char *statement, const char *file, int line, const char *function);

#ifndef __cplusplus

#define static_assert _Static_assert

#endif

#endif
