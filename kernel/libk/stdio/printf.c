#include <ctype.h>
#include <drivers/tty.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char printbuf[2048];

// helper function
char *__number_conversion(char *buf, unsigned long num, const int base, const int width, const int precision,
                          const bool left_align, const bool zero_pad, const bool upper_case, const bool negative,
                          const bool space_if_no_sign, const bool alternate_form) {
    const char *digits = upper_case ? "0123456789ABCDEF" : "0123456789abcdef";
    char tmp[32];
    int pos = 0;

    // Handle zero cases
    if (num == 0) {
        tmp[pos++] = '0';
    } else {
        // Convert number to string in reverse order
        while (num != 0) {
            const unsigned long rem = num % base;
            num /= base;
            tmp[pos++] = digits[rem];
        }
    }

    // Apply precision (minimum digits)
    while (pos < precision) {
        tmp[pos++] = '0';
    }

    // Calculate padding
    int pad_len = width - pos;
    if (negative || space_if_no_sign)
        pad_len--;
    if (alternate_form && base == 16)
        pad_len -= 2;

    // Right alignment padding
    if (!left_align && pad_len > 0) {
        memset(buf, zero_pad ? '0' : ' ', pad_len);
        buf += pad_len;
    }

    // Sign handling
    if (negative) {
        *buf++ = '-';
    } else if (space_if_no_sign) {
        *buf++ = ' ';
    }

    // Alternate form prefix
    if (alternate_form) {
        if (base == 16) {
            *buf++ = '0';
            *buf++ = upper_case ? 'X' : 'x';
        } else if (base == 8) {
            *buf++ = '0';
        }
    }

    // Write digits in the correct order
    while (--pos >= 0) {
        *buf++ = tmp[pos];
    }

    // Left alignment padding
    if (left_align && pad_len > 0) {
        memset(buf, ' ', pad_len);
        buf += pad_len;
    }

    return buf;
}

int printf(const char *restrict format, ...) {
    va_list args;

    va_start(args, format);
    const int i = vsprintf(printbuf, format, args);
    va_end(args);

    const char *p = printbuf;
    for (int j = 0; j < i; j++) {
        putck(*p++);
    }

    return i;
}

int sprintf(char *restrict str, const char *restrict format, ...) {
    va_list args;
    va_start(args, format);
    const int ret = vsprintf(str, format, args);
    va_end(args);
    return ret;
}

int snprintf(char *restrict str, const size_t size, const char *restrict format, ...) {
    va_list args;
    va_start(args, format);
    const int ret = vsnprintf(str, size, format, args);
    va_end(args);
    return ret;
}

int vprintf(const char *restrict format, const va_list ap) {
    const int len = vsprintf(printbuf, format, ap);
    const char *p = printbuf;
    for (int i = 0; i < len; i++) {
        putck(*p++);
    }
    return len;
}

int vsprintf(char *restrict buf, const char *restrict format, const va_list args) {
    const char *start = buf;

    while (*format) {
        if (*format != '%') {
            *buf++ = *format++;
            continue;
        }

        format++; // Skip '%'

        // Parse flags
        bool left_align = false;
        bool show_sign = false;
        bool space_if_no_sign = false;
        bool alternate_form = false;
        bool zero_pad = false;

        while (true) {
            switch (*format) {
                case '-':
                    left_align = true;
                    format++;
                    continue;
                case '+':
                    show_sign = true;
                    format++;
                    continue;
                case ' ':
                    space_if_no_sign = true;
                    format++;
                    continue;
                case '#':
                    alternate_form = true;
                    format++;
                    continue;
                case '0':
                    zero_pad = true;
                    format++;
                    continue;
                default:
                    break;
            }
            break;
        }

        // Parse field width
        int width = 0;
        if (isdigit(*format)) {
            width = atoi(format);
        } else if (*format == '*') {
            width = va_arg(args, int);
            format++;
            if (width < 0) {
                width = -width;
                left_align = true;
            }
        }

        // Parse precision
        int precision = -1;
        if (*format == '.') {
            format++;
            if (isdigit(*format)) {
                precision = atoi(format);
            } else if (*format == '*') {
                precision = va_arg(args, int);
                format++;
            }
            if (precision < 0)
                precision = 0;
        }

        // Handle conversion specifier
        switch (*format) {
            case 'c': {
                const char c = (char) va_arg(args, int);
                *buf++ = c;
                break;
            }

            case 's': {
                const char *s = va_arg(args, const char *);
                size_t len = s ? strlen(s) : 0;
                if (precision >= 0 && len > precision) {
                    len = precision;
                }

                if (!left_align && width > len) {
                    memset(buf, ' ', width - len);
                    buf += width - len;
                }

                if (s) {
                    memcpy(buf, s, len);
                    buf += len;
                }

                if (left_align && width > len) {
                    memset(buf, ' ', width - len);
                    buf += width - len;
                }
                break;
            }

            case 'd':
            case 'i': {
                const long num = va_arg(args, long);
                const bool negative = num < 0;
                const unsigned long unum = negative ? -num : num;

                buf = __number_conversion(buf, unum, 10, width, precision, left_align, zero_pad, 0, negative,
                                          space_if_no_sign && !negative && !show_sign, alternate_form);
                break;
            }

            case 'f':
            case 'F': {
                float f = (float) va_arg(args, double);
                char temp[1024];
                if (precision < 0)
                    precision = 6;

                // Handle negative numbers properly
                const int is_negative = f < 0;
                if (is_negative)
                    f = -f;

                // Round the number according to precision
                float round_offset = 0.5f;
                for (int i = 0; i < precision; i++)
                    round_offset /= 10.0f;
                f += round_offset;

                int whole = (int) f;
                float fraction = f - (float) whole;

                // Handle the whole part
                char *ptr = temp;
                if (whole == 0 && is_negative) {
                    *ptr++ = '-';
                    *ptr++ = '0';
                } else if (whole == 0) {
                    *ptr++ = '0';
                } else {
                    // Handle negative sign
                    if (is_negative) {
                        *ptr++ = '-';
                    }
                    // Convert whole part (reversed)
                    char *start = ptr;
                    while (whole > 0) {
                        *ptr++ = whole % 10 + '0';
                        whole /= 10;
                    }
                    // Reverse the whole part string
                    char *end = ptr - 1;
                    while (start < end) {
                        const char tmp = *start;
                        *start = *end;
                        *end = tmp;
                        start++;
                        end--;
                    }
                }

                // Handle fraction part
                if (precision > 0) {
                    *ptr++ = '.';
                    while (precision-- > 0) {
                        fraction *= 10;
                        const int digit = (int) fraction;
                        *ptr++ = digit + '0';
                        fraction -= (float) digit;
                    }
                }
                *ptr = '\0';

                const size_t len = strlen(temp);

                if (!left_align && width > len) {
                    memset(buf, ' ', width - len);
                    buf += width - len;
                }

                memcpy(buf, temp, len);
                buf += len;

                if (left_align && width > len) {
                    memset(buf, ' ', width - len);
                    buf += width - len;
                }
                break;
            }

            case 'u': {
                const unsigned long num = va_arg(args, unsigned long);
                buf = __number_conversion(buf, num, 10, width, precision, left_align, zero_pad, 0, show_sign,
                                          space_if_no_sign, alternate_form);
                break;
            }

            case 'x':
            case 'X': {
                const unsigned long num = va_arg(args, unsigned long);
                buf = __number_conversion(buf, num, 16, width, precision, left_align, zero_pad, (*format == 'X'),
                                          show_sign, space_if_no_sign, alternate_form);
                break;
            }

            case 'o': {
                const unsigned long num = va_arg(args, unsigned long);
                buf = __number_conversion(buf, num, 8, width, precision, left_align, zero_pad, 0, show_sign,
                                          space_if_no_sign, alternate_form);
                break;
            }

            case 'p': {
                void *ptr = va_arg(args, void *);
                buf = __number_conversion(buf, (unsigned long) ptr, 16, width >= 0 ? width : 2 * sizeof(void *),
                                          precision, left_align, 1, 0, 0, 0, 1);
                break;
            }

            case '%':
                *buf++ = '%';
                break;

            default:
                // Unknown specifier, copy
                *buf++ = '%';
                *buf++ = *format;
                break;
        }

        if (*format)
            format++;
    }

    *buf = '\0';
    return buf - start;
}

int vsnprintf(char *restrict str, const size_t size, const char *restrict format, va_list ap) {
    va_list ap2;
    va_copy(ap2, ap);
    // Use global printbuf to format the entire string
    const int total_len = vsprintf(printbuf, format, ap2);
    va_end(ap2);

    if (size > 0) {
        // Determine how much to copy
        size_t to_copy = total_len;
        if (to_copy >= size) {
            to_copy = size - 1;
        }
        memcpy(str, printbuf, to_copy);
        str[to_copy] = '\0';
    }

    return total_len;
}
