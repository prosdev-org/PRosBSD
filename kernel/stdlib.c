// part of x16-pros-libc

#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>

int atoi(const char *str) {
    int n = 0, neg = 0;
    while (isspace(*str)) {
        str++;
    }
    switch (*str) {
        case '-':
            neg = 1;
        case '+':
            str++;
        default:;
    }
    // Compute n as a negative number to avoid overflow on INT_MIN
    while (isdigit(*str)) {
        n = 10 * n - (*str++ - '0');
    }
    return neg ? n : -n;
}

long atol(const char *str) {
    long n = 0;
    int neg = 0;
    while (isspace(*str)) {
        str++;
    }
    switch (*str) {
        case '-':
            neg = 1;
        case '+':
            str++;
        default:;
    }
    // Compute n as a negative number to avoid overflow on LONG_MIN
    while (isdigit(*str)) {
        n = 10 * n - (*str++ - '0');
    }
    return neg ? n : -n;
}

long long atoll(const char *str) {
    long long n = 0;
    int neg = 0;
    while (isspace(*str)) {
        str++;
    }
    switch (*str) {
        case '-':
            neg = 1;
        case '+':
            str++;
        default:;
    }
    // Compute n as a negative number to avoid overflow on LLONG_MIN
    while (isdigit(*str)) {
        n = 10 * n - (*str++ - '0');
    }
    return neg ? n : -n;
}

int abs(const int x) {
    return x >= 0 ? x : -x;
}

long labs(const long x) {
    return x >= 0 ? x : -x;
}

long long llabs(const long long x) {
    return x >= 0 ? x : -x;
}

div_t div(const int num, const int den) {
    return (div_t) {num / den, num % den};
}

ldiv_t ldiv(const long num, const long den) {
    return (ldiv_t) {num / den, num % den};
}

lldiv_t lldiv(const long long num, const long long den) {
    return (lldiv_t) {num / den, num % den};
}

int64_t __divdi3(const int64_t numerator, const int64_t denominator) {
    int neg = 0;
    uint64_t ua, ub;
    if (numerator < 0) {
        ua = -(uint64_t) numerator;
        neg ^= 1;
    } else
        ua = (uint64_t) numerator;
    if (denominator < 0) {
        ub = -(uint64_t) denominator;
        neg ^= 1;
    } else
        ub = (uint64_t) denominator;

    const uint64_t uq = ua / ub;
    const int64_t q = (int64_t) uq;
    return neg ? -q : q;
}

int64_t __moddi3(const int64_t numerator, const int64_t denominator) {
    const uint64_t ua = numerator < 0 ? -(uint64_t) numerator : (uint64_t) numerator;
    const uint64_t ub = denominator < 0 ? -(uint64_t) denominator : (uint64_t) denominator;
    const uint64_t ur = ua % ub;
    const int64_t rem = (int64_t) ur;
    return numerator < 0 ? -rem : rem;
}

int64_t __divmoddi4(const int64_t numerator, const int64_t denominator, int64_t *remainder) {
    *remainder = numerator % denominator;
    return numerator / denominator;
}

uint64_t __udivdi3(const uint64_t numerator, const uint64_t denominator) {
    uint64_t q = 0;
    uint64_t r = 0;
    for (int i = 63; i >= 0; i--) {
        r = (r << 1) | ((numerator >> i) & 1);
        if (r >= denominator) {
            r -= denominator;
            q |= (uint64_t) 1 << i;
        }
    }
    return q;
}

uint64_t __umoddi3(const uint64_t numerator, const uint64_t denominator) {
    uint64_t r = 0;
    for (int i = 63; i >= 0; i--) {
        r = (r << 1) | ((numerator >> i) & 1);
        if (r >= denominator) {
            r -= denominator;
        }
    }
    return r;
}

uint64_t __udivmoddi4(const uint64_t numerator, const uint64_t denominator, uint64_t *remainder) {
    *remainder = numerator % denominator;
    return numerator / denominator;
}
