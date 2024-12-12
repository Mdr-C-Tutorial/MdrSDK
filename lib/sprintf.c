#include "ctype.h"
#include "math.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#define STB_SPRINTF_IMPLEMENTATION
#include "stb_sprintf.h"


#define CVT_BUFSZ (309 + 43)

#define MIN_RANK rank_char
#define MAX_RANK rank_longlong
#define INTMAX_RANK rank_longlong
#define SIZE_T_RANK rank_long
#define PTRDIFF_T_RANK rank_long

#define ZEROPAD 1  /* pad with zero */
#define SIGN 2     /* unsigned/signed long */
#define PLUS 4     /* show plus */
#define SPACE 8    /* space if plus */
#define LEFT 16    /* left justified */
#define SMALL 32   /* Must be 32 == 0x20 */
#define SPECIAL 64 /* 0x */

#define __do_div(n, base)                                                      \
    ({                                                                         \
        int __res;                                                             \
        __res = ((unsigned long)n) % (unsigned)base;                           \
        n = ((unsigned long)n) / (unsigned)base;                               \
        __res;                                                                 \
    })

#define EMIT(x)                                                                \
    ({                                                                         \
        if (o < n) {                                                           \
            *q++ = (x);                                                        \
        }                                                                      \
        o++;                                                                   \
    })

enum ranks {
    rank_char = -2,
    rank_short = -1,
    rank_int = 0,
    rank_long = 1,
    rank_longlong = 2,
};

enum flags {
    FL_ZERO = 0x01,   /* Zero modifier */
    FL_MINUS = 0x02,  /* Minus modifier */
    FL_PLUS = 0x04,   /* Plus modifier */
    FL_TICK = 0x08,   /* ' modifier */
    FL_SPACE = 0x10,  /* Space modifier */
    FL_HASH = 0x20,   /* # modifier */
    FL_SIGNED = 0x40, /* Number is signed */
    FL_UPPER = 0x80,  /* Upper case digits */
};

static char *cvt(double arg, int ndigits, int *decpt, int *sign, char *buf,
                 int eflag) {
    int r2;
    double fi, fj;
    char *p, *p1;

    if (ndigits < 0)
        ndigits = 0;
    if (ndigits >= CVT_BUFSZ - 1)
        ndigits = CVT_BUFSZ - 2;

    r2 = 0;
    *sign = 0;
    p = &buf[0];

    if (arg < 0) {
        *sign = 1;
        arg = -arg;
    }
    arg = modf(arg, &fi);
    p1 = &buf[CVT_BUFSZ];

    if (fi != 0) {
        p1 = &buf[CVT_BUFSZ];
        while (fi != 0) {
            fj = modf(fi / 10, &fi);
            *--p1 = (int)((fj + .03) * 10) + '0';
            r2++;
        }
        while (p1 < &buf[CVT_BUFSZ])
            *p++ = *p1++;
    } else if (arg > 0) {
        while ((fj = arg * 10) < 1) {
            arg = fj;
            r2--;
        }
    }

    p1 = &buf[ndigits];
    if (eflag == 0)
        p1 += r2;
    *decpt = r2;
    if (p1 < &buf[0]) {
        buf[0] = '\0';
        return buf;
    }

    while (p <= p1 && p < &buf[CVT_BUFSZ]) {
        arg *= 10;
        arg = modf(arg, &fj);
        *p++ = (int)fj + '0';
    }

    if (p1 >= &buf[CVT_BUFSZ]) {
        buf[CVT_BUFSZ - 1] = '\0';
        return buf;
    }
    p = p1;
    *p1 += 5;

    while (*p1 > '9') {
        *p1 = '0';
        if (p1 > buf)
            ++*--p1;
        else {
            *p1 = '1';
            (*decpt)++;
            if (eflag == 0) {
                if (p > buf)
                    *p = '0';
                p++;
            }
        }
    }

    *p = '\0';
    return buf;
}

static void cfltcvt(double value, char *buffer, char fmt, int precision) {
    int decpt, sign, exp, pos;
    char *digits = 0;
    char cvtbuf[CVT_BUFSZ];
    int capexp = 0;
    int magnitude;

    if (fmt == 'G' || fmt == 'E') {
        capexp = 1;
        fmt += 'a' - 'A';
    }

    if (fmt == 'g') {
        digits = cvt(value, precision, &decpt, &sign, cvtbuf, 1);

        magnitude = decpt - 1;
        if (magnitude < -4 || magnitude > precision - 1) {
            fmt = 'e';
            precision -= 1;
        } else {
            fmt = 'f';
            precision -= decpt;
        }
    }

    if (fmt == 'e') {
        digits = cvt(value, precision + 1, &decpt, &sign, cvtbuf, 1);

        if (sign)
            *buffer++ = '-';
        *buffer++ = *digits;
        if (precision > 0)
            *buffer++ = '.';
        memcpy(buffer, digits + 1, precision);
        buffer += precision;
        *buffer++ = capexp ? 'E' : 'e';

        if (decpt == 0) {
            if (value == 0.0)
                exp = 0;
            else
                exp = -1;
        } else
            exp = decpt - 1;

        if (exp < 0) {
            *buffer++ = '-';
            exp = -exp;
        } else
            *buffer++ = '+';

        buffer[2] = (exp % 10) + '0';
        exp = exp / 10;
        buffer[1] = (exp % 10) + '0';
        exp = exp / 10;
        buffer[0] = (exp % 10) + '0';
        buffer += 3;
    } else if (fmt == 'f') {
        digits = cvt(value, precision, &decpt, &sign, cvtbuf, 0);

        if (sign)
            *buffer++ = '-';
        if (*digits) {
            if (decpt <= 0) {
                *buffer++ = '0';
                *buffer++ = '.';
                for (pos = 0; pos < -decpt; pos++)
                    *buffer++ = '0';
                while (*digits)
                    *buffer++ = *digits++;
            } else {
                pos = 0;
                while (*digits) {
                    if (pos++ == decpt)
                        *buffer++ = '.';
                    *buffer++ = *digits++;
                }
            }
        } else {
            *buffer++ = '0';
            if (precision > 0) {
                *buffer++ = '.';
                for (pos = 0; pos < precision; pos++)
                    *buffer++ = '0';
            }
        }
    }

    *buffer = '\0';
}

static void forcdecpt(char *buffer) {
    while (*buffer) {
        if (*buffer == '.')
            return;
        if (*buffer == 'e' || *buffer == 'E')
            break;
        buffer++;
    }

    if (*buffer) {
        int n = strlen(buffer);
        while (n > 0) {
            buffer[n + 1] = buffer[n];
            n--;
        }

        *buffer = '.';
    } else {
        *buffer++ = '.';
        *buffer = '\0';
    }
}

static void cropzeros(char *buffer) {
    char *stop;

    while (*buffer && *buffer != '.')
        buffer++;
    if (*buffer++) {
        while (*buffer && *buffer != 'e' && *buffer != 'E')
            buffer++;
        stop = buffer--;
        while (*buffer == '0')
            buffer--;
        if (*buffer == '.')
            buffer--;
        while ((*++buffer = *stop++))
            ;
    }
}

static size_t format_float(char *q, size_t n, double val, enum flags flags,
                           char fmt, int width, int prec) {
    size_t o = 0;
    char tmp[CVT_BUFSZ];
    char c, sign;
    int len, i;

    if (flags & FL_MINUS)
        flags &= ~FL_ZERO;

    c = (flags & FL_ZERO) ? '0' : ' ';
    sign = 0;
    if (flags & FL_SIGNED) {
        if (val < 0.0) {
            sign = '-';
            val = -val;
            width--;
        } else if (flags & FL_PLUS) {
            sign = '+';
            width--;
        } else if (flags & FL_SPACE) {
            sign = ' ';
            width--;
        }
    }

    if (prec < 0)
        prec = 6;
    else if (prec == 0 && fmt == 'g')
        prec = 1;

    cfltcvt(val, tmp, fmt, prec);

    if ((flags & FL_HASH) && prec == 0)
        forcdecpt(tmp);

    if (fmt == 'g' && !(flags & FL_HASH))
        cropzeros(tmp);

    len = strlen(tmp);
    width -= len;

    if (!(flags & (FL_ZERO | FL_MINUS)))
        while (width-- > 0)
            EMIT(' ');

    if (sign)
        EMIT(sign);

    if (!(flags & FL_MINUS)) {
        while (width-- > 0)
            EMIT(c);
    }

    for (i = 0; i < len; i++)
        EMIT(tmp[i]);

    while (width-- > 0)
        EMIT(' ');

    return o;
}

static size_t format_int(char *q, size_t n, uintmax_t val, enum flags flags,
                         int base, int width, int prec) {
    char *qq;
    size_t o = 0, oo;
    static const char lcdigits[] = "0123456789abcdef";
    static const char ucdigits[] = "0123456789ABCDEF";
    const char *digits;
    uintmax_t tmpval;
    int minus = 0;
    int ndigits = 0, nchars;
    int tickskip, b4tick;

    /*
     * Select type of digits
     */
    digits = (flags & FL_UPPER) ? ucdigits : lcdigits;

    /*
     * If signed, separate out the minus
     */
    if ((flags & FL_SIGNED) && ((intmax_t)val < 0)) {
        minus = 1;
        val = (uintmax_t)(-(intmax_t)val);
    }

    /*
     * Count the number of digits needed.  This returns zero for 0
     */
    tmpval = val;
    while (tmpval) {
        tmpval /= base;
        ndigits++;
    }

    /*
     * Adjust ndigits for size of output
     */
    if ((flags & FL_HASH) && (base == 8)) {
        if (prec < ndigits + 1)
            prec = ndigits + 1;
    }

    if (ndigits < prec) {
        ndigits = prec; /* Mandatory number padding */
    } else if (val == 0) {
        ndigits = 1; /* Zero still requires space */
    }

    /*
     * For ', figure out what the skip should be
     */
    if (flags & FL_TICK) {
        tickskip = (base == 16) ? 4 : 3;
    } else {
        tickskip = ndigits; /* No tick marks */
    }

    /*
     * Tick marks aren't digits, but generated by the number converter
     */
    ndigits += (ndigits - 1) / tickskip;

    /*
     * Now compute the number of nondigits
     */
    nchars = ndigits;

    if (minus || (flags & (FL_PLUS | FL_SPACE)))
        nchars++; /* Need space for sign */
    if ((flags & FL_HASH) && (base == 16)) {
        nchars += 2; /* Add 0x for hex */
    }

    /*
     * Emit early space padding
     */
    if (!(flags & (FL_MINUS | FL_ZERO)) && (width > nchars)) {
        while (width > nchars) {
            EMIT(' ');
            width--;
        }
    }

    /*
     * Emit nondigits
     */
    if (minus)
        EMIT('-');
    else if (flags & FL_PLUS)
        EMIT('+');
    else if (flags & FL_SPACE)
        EMIT(' ');

    if ((flags & FL_HASH) && (base == 16)) {
        EMIT('0');
        EMIT((flags & FL_UPPER) ? 'X' : 'x');
    }

    /*
     * Emit zero padding
     */
    if (((flags & (FL_MINUS | FL_ZERO)) == FL_ZERO) && (width > ndigits)) {
        while (width > nchars) {
            EMIT('0');
            width--;
        }
    }

    /*
     * Generate the number.  This is done from right to left
     */
    q += ndigits; /* Advance the pointer to end of number */
    o += ndigits;
    qq = q;
    oo = o; /* Temporary values */

    b4tick = tickskip;
    while (ndigits > 0) {
        if (!b4tick--) {
            qq--;
            oo--;
            ndigits--;
            if (oo < n)
                *qq = '_';
            b4tick = tickskip - 1;
        }
        qq--;
        oo--;
        ndigits--;
        if (oo < n)
            *qq = digits[val % base];
        val /= base;
    }

    /*
     * Emit late space padding
     */
    while ((flags & FL_MINUS) && (width > nchars)) {
        EMIT(' ');
        width--;
    }

    return o;
}

static char *number(char *str, long num, int base, int size, int precision,
                    int type) {
    static const char digits[16] = "0123456789ABCDEF";

    char tmp[66];
    char c, sign, locase;
    int i;

    locase = (type & SMALL);
    if (type & LEFT)
        type &= ~ZEROPAD;
    if (base < 2 || base > 16)
        return NULL;
    c = (type & ZEROPAD) ? '0' : ' ';
    sign = 0;
    if (type & SIGN) {
        if (num < 0) {
            sign = '-';
            num = -num;
            size--;
        } else if (type & PLUS) {
            sign = '+';
            size--;
        } else if (type & SPACE) {
            sign = ' ';
            size--;
        }
    }
    if (type & SPECIAL) {
        if (base == 16)
            size -= 2;
        else if (base == 8)
            size--;
    }
    i = 0;
    if (num == 0)
        tmp[i++] = '0';
    else
        while (num != 0)
            tmp[i++] = (digits[__do_div(num, base)] | locase);
    if (i > precision)
        precision = i;
    size -= precision;
    if (!(type & (ZEROPAD + LEFT)))
        while (size-- > 0)
            *str++ = ' ';
    if (sign)
        *str++ = sign;
    if (type & SPECIAL) {
        if (base == 8)
            *str++ = '0';
        else if (base == 16) {
            *str++ = '0';
            *str++ = ('X' | locase);
        }
    }
    if (!(type & LEFT))
        while (size-- > 0)
            *str++ = c;
    while (i < precision--)
        *str++ = '0';
    while (i-- > 0)
        *str++ = tmp[i];
    while (size-- > 0)
        *str++ = ' ';
    return str;
}

int sprintf(char *buf, const char *fmt, ...) {
    va_list args;
    int i;
    va_start(args, fmt);
    i = stbsp_vsprintf(buf, fmt, args);
    va_end(args);
    return i;
}

int vsprintf(char *buf, const char *fmt, va_list args) {
    //return stbsp_vsprintf(buf,fmt,args);
    int len;
    unsigned long num;
    int i, base;
    char *str;
    const char *s;

    int flags; /* flags to number() */

    int field_width; /* width of output field */
    int precision;   /* min. # of digits for integers; max
              number of chars for from string */
    int qualifier;   /* 'h', 'l', or 'L' for integer fields */

    for (str = buf; *fmt; ++fmt) {
        if (*fmt != '%') {
            *str++ = *fmt;
            continue;
        }

        /* process flags */
        flags = 0;
        repeat:
        ++fmt; /* this also skips first '%' */
        switch (*fmt) {
            case '-':
                flags |= LEFT;
                goto repeat;
            case '+':
                flags |= PLUS;
                goto repeat;
            case ' ':
                flags |= SPACE;
                goto repeat;
            case '#':
                flags |= SPECIAL;
                goto repeat;
            case '0':
                flags |= ZEROPAD;
                goto repeat;
        }

        /* get field width */
        field_width = -1;
        if (isdigit(*fmt))
            field_width = atoi(&fmt);
        else if (*fmt == '*') {
            ++fmt;
            /* it's the next argument */
            field_width = va_arg(args, int);
            if (field_width < 0) {
                field_width = -field_width;
                flags |= LEFT;
            }
        }

        /* get the precision */
        precision = -1;
        if (*fmt == '.') {
            ++fmt;
            if (isdigit(*fmt))
                precision = atoi(&fmt);
            else if (*fmt == '*') {
                ++fmt;
                /* it's the next argument */
                precision = va_arg(args, int);
            }
            if (precision < 0)
                precision = 0;
        }

        /* get the conversion qualifier */
        qualifier = -1;
        if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L') {
            qualifier = *fmt;
            ++fmt;
        }

        /* default base */
        base = 10;

        switch (*fmt) {
            case 'c':
                if (!(flags & LEFT))
                    while (--field_width > 0)
                        *str++ = ' ';
                *str++ = (unsigned char)va_arg(args, int);
                while (--field_width > 0)
                    *str++ = ' ';
                continue;

            case 's':
                s = va_arg(args, char *);
                len = strnlen(s, precision);

                if (!(flags & LEFT))
                    while (len < field_width--)
                        *str++ = ' ';
                for (i = 0; i < len; ++i)
                    *str++ = *s++;
                while (len < field_width--)
                    *str++ = ' ';
                continue;

            case 'p':
                if (field_width == -1) {
                    field_width = 2 * sizeof(void *);
                    flags |= ZEROPAD;
                }
                str = number(str, (unsigned long)va_arg(args, void *), 16,
                field_width, precision, flags);
                continue;

            case 'n':
                if (qualifier == 'l') {
                    long *ip = va_arg(args, long *);
                    *ip = (str - buf);
                } else {
                    int *ip = va_arg(args, int *);
                    *ip = (str - buf);
                }
                continue;

            case '%':
                *str++ = '%';
                continue;

                /* integer number formats - set up the flags and "break" */
            case 'o':
                base = 8;
                break;

            case 'x':
                flags |= SMALL;
            case 'X':
                base = 16;
                break;

            case 'd':
            case 'i':
                flags |= SIGN;
            case 'u':
                break;
            case 'f':
                str = ftoa(va_arg(args, double), str, precision);
                break;

            default:
                *str++ = '%';
                if (*fmt)
                    *str++ = *fmt;
                else
                    --fmt;
                continue;
        }
        if (qualifier == 'l')
            num = va_arg(args, unsigned long);
        else if (qualifier == 'h') {
            num = (unsigned short)va_arg(args, int);
            if (flags & SIGN)
                num = (short)num;
        } else if (flags & SIGN)
            num = va_arg(args, int);
        else
        num = va_arg(args, unsigned int);
        str = number(str, num, base, field_width, precision, flags);
    }
    *str = '\0';
    return str - buf;
}

int vsnprintf(char *buf, size_t n, const char *fmt, va_list ap) {
    return stbsp_vsnprintf(buf,n,fmt,ap);
}