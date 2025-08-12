#ifndef STDLIB_H
#define STDLIB_H

#include <stddef.h>

int atoi(const char *);
long atol(const char *);
long long atoll(const char *);
// double atof(const char *);

// float strtof(const char *__restrict, char **__restrict);
// double strtod(const char *__restrict, char **__restrict);
// long double strtold(const char *__restrict, char **__restrict);

// long strtol(const char *__restrict, char **__restrict, int);
// unsigned long strtoul(const char *__restrict, char **__restrict, int);
// long long strtoll(const char *__restrict, char **__restrict, int);
// unsigned long long strtoull(const char *__restrict, char **__restrict, int);

// int rand(void);
// void srand(unsigned);

void *malloc(size_t);
void *calloc(size_t, size_t);
// void *realloc(void *, size_t);
void free(void *);

void *mallocl(size_t);

// void abort(void);
// int atexit(void (*)(void));
// void exit(int);
// void _Exit(int);

// char *getenv(const char *);

// int system(const char *);

// void *bsearch(const void *, const void *, size_t, size_t, int (*)(const void *, const void *));
// void qsort(void *, size_t, size_t, int (*)(const void *, const void *));

int abs(int);
long labs(long);
long long llabs(long long);

typedef struct {
    int quot, rem;
} div_t;

typedef struct {
    long quot, rem;
} ldiv_t;

typedef struct {
    long long quot, rem;
} lldiv_t;

div_t div(int, int);
ldiv_t ldiv(long, long);
lldiv_t lldiv(long long, long long);

// int mblen(const char *, size_t);
// int mbtowc(wchar_t *__restrict, const char *__restrict, size_t);
// int wctomb(char *, wchar_t);
// size_t mbstowcs(wchar_t *__restrict, const char *__restrict, size_t);
// size_t wcstombs(char *__restrict, const wchar_t *__restrict, size_t);

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

#endif
