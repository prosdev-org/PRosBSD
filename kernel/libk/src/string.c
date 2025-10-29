#include <stddef.h>
#include <string.h>

void *memcpy(void *restrict dst, const void *restrict src, size_t const n) {
    unsigned char *d = dst;
    const unsigned char *s = src;
    for (size_t i = 0; i < n; i++) {
        d[i] = s[i];
    }
    return dst;
}

void *memmove(void *dst, const void *src, const size_t n) {
    unsigned char *d = dst;
    const unsigned char *s = src;
    if (d == s) {
        return dst;
    }
    if (d < s) {
        for (size_t i = 0; i < n; i++) {
            d[i] = s[i];
        }
    } else {
        for (size_t i = n; i > 0; i--) {
            d[i - 1] = s[i - 1];
        }
    }
    return dst;
}

void *memset(void *dst, int const ch, size_t const n) {
    unsigned char *d = dst;
    for (size_t i = 0; i < n; i++) {
        d[i] = ch;
    }
    return dst;
}

int memcmp(const void *lhs, const void *rhs, size_t n) {
    const unsigned char *l = lhs, *r = rhs;
    for (; n && *l == *r; n--, l++, r++) {
    }
    return n ? *l - *r : 0;
}

void *memchr(const void *src, int ch, size_t n) {
    const unsigned char *s = src;
    ch = (unsigned char) ch;
    for (; n && *s != ch; s++, n--) {
    }
    return n ? (void *) s : NULL;
}

char *strcpy(char *restrict dst, const char *restrict src) {
    char *d = dst;
    const char *s = src;
    while ((*d++ = *s++)) {
    }
    return dst;
}

char *strncpy(char *restrict dst, const char *restrict src, const size_t n) {
    size_t i = 0;
    for (; i < n && src[i] != '\0'; i++) {
        dst[i] = src[i];
    }
    for (; i < n; i++) {
        dst[i] = '\0';
    }
    return dst;
}

char *strcat(char *restrict dst, const char *restrict src) {
    char *d = dst;
    const char *s = src;
    while (*d++) {
    }
    while ((*d++ = *s++)) {
    }
    return dst;
}

char *strncat(char *restrict dst, const char *restrict src, const size_t n) {
    char *d = dst;
    while (*d++) {
    }
    for (size_t i = 0; i < n && src[i] != '\0'; i++) {
        d[i] = src[i];
    }
    return dst;
}

int strcmp(const char *lhs, const char *rhs) {
    for (; *lhs == *rhs && *lhs; lhs++, rhs++) {
    }
    return *(unsigned char *) lhs - *(unsigned char *) rhs;
}

int strncmp(const char *lhs, const char *rhs, size_t n) {
    const unsigned char *l = (void *) lhs, *r = (void *) rhs;
    if (!n--)
        return 0;
    for (; *l && *r && n && *l == *r; l++, r++, n--) {
    }
    return *l - *r;
}

int strcasecmp(const char *s1, const char *s2) {
    while (*s1 && *s2) {
        char c1 = *s1;
        char c2 = *s2;
        if (c1 >= 'a' && c1 <= 'z')
            c1 -= 32;
        if (c2 >= 'a' && c2 <= 'z')
            c2 -= 32;
        if (c1 != c2)
            return c1 - c2;
        s1++;
        s2++;
    }
    return *s1 - *s2;
}

int strcoll(const char *lhs, const char *rhs) {
    return strcmp(lhs, rhs);
}

size_t strxfrm(char *dest, const char *src, const size_t n) {
    const size_t l = strlen(src);
    if (n > 1) {
        strcpy(dest, src);
    }
    return l;
}

char *strchr(const char *src, const int ch) {
    const char *s = src;
    while (*s++ != ch && *s != '\0') {
    }
    if (*--s == ch) {
        return (char *) s;
    }
    return NULL;
}

char *strrchr(const char *src, const int ch) {
    const char *s = src;
    const char *c = NULL;
    for (; *s != '\0'; s++) {
        if (*s == ch) {
            c = s;
        }
    }
    return (char *) c;
}

size_t strcspn(const char *dst, const char *src) {
    for (size_t i = 0; src[i] != '\0'; i++) {
        if (strchr(dst, src[i])) {
            return i;
        }
    }
    return 0;
}

char *strpbrk(const char *dst, const char *brkset) {
    const char *d = dst;
    while (*dst != '\0') {
        if (*dst == *brkset) {
            return (char *) d;
        }
    }
    return NULL;
}

char *strstr(const char *str, const char *substr) {
    const char *s = str;
    while (*s != '\0') {
        const char *s1 = s;
        const char *s2 = substr;
        while (*s1 && *s2 && *s1 == *s2) {
            s1++;
            s2++;
        }
        if (*s2 == '\0') {
            return (char *) s;
        }
        s++;
    }
    return NULL;
}

char *strtok(char *restrict str, const char *restrict delim) {
    static char *last;
    if (str != NULL) {
        last = str;
    }
    if (last == NULL) {
        return NULL;
    }
    while (*last && strchr(delim, *last)) {
        last++;
    }
    if (*last == '\0') {
        return NULL;
    }
    char *tokstart = last;
    while (*last && !strchr(delim, *last)) {
        last++;
    }
    if (*last != '\0') {
        *last = '\0';
        last++;
    } else {
        last = NULL;
    }
    return tokstart;
}

size_t strlen(const char *str) {
    const char *s = str;
    while (*s != '\0') {
        s++;
    }
    return s - str;
}
