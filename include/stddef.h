// part of x16-pros-libc

#ifndef STDDEF_H
#define STDDEF_H

#ifndef _SIZE_T_DEFINED
#define _SIZE_T_DEFINED
typedef __SIZE_TYPE__ size_t;
#endif //_SIZE_T_DEFINED

#ifndef _PTRDIFF_T_DEFINED
#define _PTRDIFF_T_DEFINED
typedef __PTRDIFF_TYPE__ ptrdiff_t;
#endif //_PTRDIFF_T_DEFINED

#ifndef NULL
#define NULL ((void *) 0)
#endif // NULL

#ifndef _WCHAR_T_DEFINED
#define _WCHAR_T_DEFINED
typedef __WCHAR_TYPE__ wchar_t;
#endif //_WCHAR_T_DEFINED

#define offsetof(type, member) ((size_t) &(((type *) 0)->member))

#endif // STDDEF_H
