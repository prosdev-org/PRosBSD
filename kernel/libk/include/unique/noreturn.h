#ifndef LIBK_UNIQUE_NORETURN_H
#define LIBK_UNIQUE_NORETURN_H

#ifdef __cplusplus
#define noreturn [[noreturn]]
#else
#define noreturn _Noreturn
#endif

#endif
