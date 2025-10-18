#ifndef STDIO_H
#define STDIO_H

#include <stdarg.h>
#include <stddef.h>

#ifndef EOF
#define EOF (-1)
#endif

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

typedef struct FILE {
    int fd;
    int flags;
    unsigned char *buf;
    size_t buf_size;
    size_t buf_pos;
} FILE;

// FILE *fopen(const char *restrict, const char *restrict);
// FILE *freopen(const char *restrict, const char *restrict, FILE *restrict);
// int fclose(FILE *);

// int remove(const char *);
// int rename(const char *, const char *);

// int feof(FILE *);
// int ferror(FILE *);
// int fflush(FILE *);
// void clearerr(FILE *);

// int fseek(FILE *, long, int);
// long ftell(FILE *);
// void rewind(FILE *);

// int fgetpos(FILE *restrict, fpos_t *restrict);
// int fsetpos(FILE *, const fpos_t *);

// size_t fread(void *restrict, size_t, size_t, FILE *restrict);
// size_t fwrite(const void *restrict, size_t, size_t, FILE *restrict);

// int fgetc(FILE *);
// int getc(FILE *);
int getchar();
// int ungetc(int, FILE *);

// int fputc(int, FILE *);
// int putc(int, FILE *);
int putchar(int);

// char *fgets(char *restrict, int, FILE *restrict);
char *gets(char *);

// int fputs(const char *restrict, FILE *restrict);
int puts(const char *);

int printf(const char *restrict, ...);
// int fprintf(FILE *restrict, const char *restrict, ...);
int sprintf(char *restrict, const char *restrict, ...);
int snprintf(char *restrict, size_t, const char *restrict, ...);

int vprintf(const char *restrict, va_list);
// int vfprintf(FILE *restrict, const char *restrict, va_list);
int vsprintf(char *restrict, const char *restrict, va_list);
int vsnprintf(char *restrict, size_t, const char *restrict, va_list);

// int scanf(const char *restrict, ...);
// int fscanf(FILE *restrict, const char *restrict, ...);
// int sscanf(const char *restrict, const char *restrict, ...);
// int vscanf(const char *restrict, va_list);
// int vfscanf(FILE *restrict, const char *restrict, va_list);
// int vsscanf(const char *restrict, const char *restrict, va_list);

// void perror(const char *);

// int setvbuf(FILE *restrict, char *restrict, int, size_t);
// void setbuf(FILE *restrict, char *restrict);

// char *tmpnam(char *);
// FILE *tmpfile(void);

#endif
