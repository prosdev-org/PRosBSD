#ifndef LIBK_CTYPE_H
#define LIBK_CTYPE_H

#include <unique/extern_c.h>

EXTERN_C int isalnum(int ch);
EXTERN_C int isalpha(int ch);
EXTERN_C int islower(int ch);
EXTERN_C int isupper(int ch);
EXTERN_C int isdigit(int ch);
EXTERN_C int isxdigit(int ch);
EXTERN_C int iscntrl(int ch);
EXTERN_C int isgraph(int ch);
EXTERN_C int isspace(int ch);
EXTERN_C int isprint(int ch);
EXTERN_C int ispunct(int ch);
EXTERN_C int tolower(int ch);
EXTERN_C int toupper(int ch);

#endif
