#include <stdint.h>
#include <string.h>

char *strcpy(char *restrict dest, const char *restrict src) {
    return memmove(dest, src, strlen(src) + 1);
}

char *strncpy(char *restrict dest, const char *restrict src, const size_t count) {
    const size_t offset = strnlen(src, count);
    memmove(dest, src, offset);

    if (count > offset) {
        memset(&dest[offset], '\0', count - offset);
    }

    return dest;
}

char *strcat(char *restrict dest, const char *restrict src) {
    return strcpy(&dest[strlen(dest)], src);
}

char *strncat(char *restrict dest, const char *restrict src, const size_t count) {
    const size_t src_len = strlen(src);
    const size_t dest_len = strlen(dest);

    if (count < src_len) {
        strncpy(&dest[dest_len], src, count);
        dest[dest_len + count] = '\0';
    } else {
        strcpy(&dest[dest_len], src);
    }

    return dest;
}

size_t strlen(const char *str) {
    for (size_t i = 0;; i++) {
        if (str[i] == '\0') {
            return i;
        }
    }
}

size_t strnlen(const char *str, const size_t count) {
    for (size_t i = 0; i < count; i++) {
        if (str[i] == '\0') {
            return i;
        }
    }

    return count;
}

int strcmp(const char *lhs, const char *rhs) {
    const unsigned char *l = (unsigned char *) lhs;
    const unsigned char *r = (unsigned char *) rhs;

    size_t i = 0;
    for (; l[i] != '\0'; i++) {
        if (l[i] != r[i]) {
            break;
        }
    }

    return l[i] - r[i];
}

int strncmp(const char *lhs, const char *rhs, const size_t count) {
    if (count == 0) {
        return 0;
    }

    const unsigned char *l = (unsigned char *) lhs;
    const unsigned char *r = (unsigned char *) rhs;

    size_t i = 0;
    for (; i < count && l[i] != '\0'; i++) {
        if (l[i] != r[i]) {
            break;
        }
    }

    return l[i] - r[i];
}

char *strchr(const char *str, const int ch) {
    if ((char) ch == '\0') {
        return (char *) &str[strlen(str)];
    }

    const unsigned char *s = (unsigned char *) str;

    for (size_t i = 0; s[i] != '\0'; i++) {
        if (s[i] == (char) ch) {
            return (char *) &s[i];
        }
    }

    return NULL;
}

char *strrchr(const char *str, const int ch) {
    if ((char) ch == '\0') {
        return (char *) &str[strlen(str)];
    }

    const unsigned char *s = (unsigned char *) str;
    const size_t len = strlen(str);

    for (size_t i = 0; i < len; i++) {
        if (s[len - 1 - i] == (char) ch) {
            return (char *) &s[len - 1 - i];
        }
    }

    return NULL;
}

void *memchr(const void *ptr, const int ch, const size_t count) {
    const unsigned char *s = ptr;

    for (size_t i = 0; i < count; i++) {
        if (s[i] == ch) {
            return (void *) &s[i];
        }
    }

    return NULL;
}

int memcmp(const void *lhs, const void *rhs, const size_t count) {
    const unsigned char *l = lhs;
    const unsigned char *r = rhs;

    size_t i = 0;
    for (; i < count; i++) {
        if (l[i] != r[i]) {
            break;
        }
    }

    return l[i] - r[i];
}

void *memset(void *dest, const int ch, const size_t count) {
    unsigned char *d = dest;

    for (size_t i = 0; i < count; i++) {
        d[i] = ch;
    }

    return dest;
}

void *memcpy(void *dest, const void *src, const size_t count) {
    unsigned char *d = dest;
    const unsigned char *s = src;

    for (size_t i = 0; i < count; i++) {
        d[i] = s[i];
    }

    return dest;
}

void *memmove(void *dest, const void *src, const size_t count) {
    if (dest == src) {
        return dest;
    }

    if ((uintptr_t) dest - (uintptr_t) src + count >= 2 * count) {
        return memcpy(dest, src, count);
    }

    unsigned char *d = dest;
    const unsigned char *s = src;

    if (d < s) {
        for (size_t i = 0; i < count; i++) {
            d[i] = s[i];
        }
    } else {
        for (size_t i = 0; i < count; i++) {
            d[count - 1 - i] = s[count - 1 - i];
        }
    }

    return dest;
}
