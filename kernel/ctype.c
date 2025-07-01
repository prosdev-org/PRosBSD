// part of x16-pros-libc

#include <ctype.h>

int isalnum(const int c) {
    return isalpha(c) || isdigit(c);
}

int isalpha(const int c) {
    return ((unsigned) c | 32) - 'a' < 26;
}

int isblank(const int c) {
    return c == ' ' || c == '\t';
}

int iscntrl(const int c) {
    return (unsigned)c < 0x20 || c == 0x7f;
}

int isdigit(const int c) {
    return (unsigned) c - '0' < 10;
}

int isgraph(const int c) {
    return (unsigned) c - 0x21 < 0x5e;
}

int islower(const int c) {
    return (unsigned) c - 'a' < 26;
}

int isprint(const int c) {
    return (unsigned) c - 0x20 < 0x5f;
}

int ispunct(const int c) {
    return isgraph(c) && !isalnum(c);
}

int isspace(const int c) {
    return c == ' ' || (unsigned) c - '\t' < 5;
}

int isupper(const int c) {
    return (unsigned) c - 'A' < 26;
}

int isdigitx(const int c) {
    return isdigit(c) || ((unsigned) c | 32) - 'a' < 6;
}

int tolower(const int c) {
    if (isupper(c)) {
        return c | 32;
    }
    return c;
}

int toupper(const int c) {
    if (isupper(c)) {
        return c & 0x5f;
    }
    return c;
}