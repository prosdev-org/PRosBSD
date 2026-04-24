#include <ctype.h>
#include <stdbool.h>

int isalnum(const int ch) {
    return isalpha(ch) || isdigit(ch);
}

int isalpha(const int ch) {
    return islower(ch) || isupper(ch);
}

int islower(const int ch) {
    return (ch >= 'a') && (ch <= 'z');
}

int isupper(const int ch) {
    return (ch >= 'A') && (ch <= 'Z');
}

int isdigit(const int ch) {
    return (ch >= '0') && (ch <= '9');
}

int isxdigit(const int ch) {
    return isdigit(ch) ||
           ((ch >= 'a') && (ch <= 'f')) ||
           ((ch >= 'A') && (ch <= 'F'));
}

int iscntrl(const int ch) {
    return ((ch >= 0) && (ch <= 0x1f)) || (ch == 0x7f);
}

int isgraph(const int ch) {
    return isalnum(ch) || ispunct(ch);
}

int isspace(const int ch) {
    switch (ch) {
        case (' '):
        case ('\f'):
        case ('\n'):
        case ('\r'):
        case ('\t'):
        case ('\v'):
            return true;
        default:
            return false;
    }
}

int isprint(const int ch) {
    return isgraph(ch) || (ch == ' ');
}

int ispunct(const int ch) {
    return ((ch >= '!') && (ch <= '/')) ||
           ((ch >= ':') && (ch <= '@')) ||
           ((ch >= '[') && (ch <= '`')) ||
           ((ch >= '{') && (ch <= '~'));
}

int tolower(const int ch) {
    if (!isupper(ch)) {
        return ch;
    }

    return ch + 0x20;
}

int toupper(const int ch) {
    if (!islower(ch)) {
        return ch;
    }

    return ch - 0x20;
}
