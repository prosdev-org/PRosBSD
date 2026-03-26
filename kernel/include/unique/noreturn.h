#ifndef LIBK_UNIQUE_NORETURN_H
#define LIBK_UNIQUE_NORETURN_H

#ifdef __cplusplus
#define NORETURN [[noreturn]]
#else
#define NORETURN _Noreturn
#endif

#endif
