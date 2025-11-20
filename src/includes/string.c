/**
 * libc-модуль управления строковыми данными.
 */
#include "string.h"

static void buf_putc(char* buffer, size_t size, size_t* pos, char c) {
    if (*pos + 1 < size)
        buffer[*pos] = c;
    (*pos)++;
}

static void buf_puts(char* buffer, size_t size, size_t* pos, const char* s, size_t limit) {
    size_t count = 0;
    while (*s && (limit == (size_t)-1 || count < limit)) {
        buf_putc(buffer, size, pos, *s++);
        count++;
    }
}

static size_t strlen(const char* s) {
    size_t n = 0;
    while (s[n]) n++;
    return n;
}

static int utoa64(uint64_t value, char* buf, int base, int uppercase) {
    static const char dig_l[] = "0123456789abcdef";
    static const char dig_u[] = "0123456789ABCDEF";
    const char* digits = uppercase ? dig_u : dig_l;

    int pos = 0;
    if (value == 0) {
        buf[pos++] = '0';
        buf[pos] = 0;
        return pos;
    }

    while (value > 0) {
        buf[pos++] = digits[value % base];
        value /= base;
    }

    for (int i = 0, j = pos - 1; i < j; i++, j--) {
        char t = buf[i];
        buf[i] = buf[j];
        buf[j] = t;
    }

    buf[pos] = 0;
    return pos;
}

static int itoa64(int64_t value, char* buf, int base) {
    if (value < 0) {
        buf[0] = '-';
        int len = utoa64(-value, buf + 1, base, 0);
        return len + 1;
    } else {
        return utoa64(value, buf, base, 0);
    }
}

static int ftoa(double d, char* buf, int precision) {
    if (precision < 0)
        precision = 6;

    char* start = buf;

    if (d < 0) {
        *buf++ = '-';
        d = -d;
    }

    uint64_t ip = (uint64_t)d;
    double frac = d - (double)ip;

    buf += utoa64(ip, buf, 10, 0);

    if (precision == 0) {
        *buf = 0;
        return buf - start;
    }

    *buf++ = '.';

    for (int i = 0; i < precision; i++) {
        frac *= 10.0;
        int digit = (int)frac;
        frac -= digit;
        *buf++ = '0' + digit;
    }

    *buf = 0;
    return buf - start;
}

uint64_t __udivmoddi4(uint64_t num, uint64_t den, uint64_t* rem)
{
    // Деление на ноль — undefined, но чтобы не бахнуло:
    if (den == 0) {
        if (rem) *rem = 0;
        return 0;
    }

    uint64_t quotient = 0;
    uint64_t qbit = 1;

    // Нормализация: двигаем делитель так, чтобы его MSB был рядом с числителем
    while ((int64_t)den >= 0 && den < num) {
        den <<= 1;
        qbit <<= 1;
    }

    while (qbit) {
        if (num >= den) {
            num -= den;
            quotient |= qbit;
        }
        den >>= 1;
        qbit >>= 1;
    }

    if (rem)
        *rem = num;

    return quotient;
}


int vsnprintf(char* buffer, size_t size, const char* fmt, va_list args) {
    size_t pos = 0;

    while (*fmt) {
        if (*fmt != '%') {
            buf_putc(buffer, size, &pos, *fmt++);
            continue;
        }

        fmt++;

        int left = 0;
        int plus = 0;
        int space = 0;
        int zero = 0;

        while (1) {
            if (*fmt == '-') { left = 1; fmt++; continue; }
            if (*fmt == '+') { plus = 1; fmt++; continue; }
            if (*fmt == ' ') { space = 1; fmt++; continue; }
            if (*fmt == '0') { zero = 1; fmt++; continue; }
            break;
        }

        int width = 0;
        while (*fmt >= '0' && *fmt <= '9')
            width = width * 10 + (*fmt++ - '0');

        int precision = -1;
        if (*fmt == '.') {
            fmt++;
            precision = 0;
            while (*fmt >= '0' && *fmt <= '9')
                precision = precision * 10 + (*fmt++ - '0');
        }

        int long_count = 0;
        while (*fmt == 'l') {
            long_count++;
            fmt++;
        }

        char tmp[256];
        const char* out = tmp;

        switch (*fmt) {
        case 's': {
            out = va_arg(args, char*);
            if (!out) out = "(null)";
            break;
        }

        case 'c': {
            tmp[0] = (char)va_arg(args, int);
            tmp[1] = 0;
            break;
        }

        case 'd':
        case 'i': {
            int64_t v = (long_count == 2) ? va_arg(args, long long)
                       : (long_count == 1) ? va_arg(args, long)
                       : va_arg(args, int);
            itoa64(v, tmp, 10);
            break;
        }

        case 'u': {
            uint64_t v = (long_count == 2) ? va_arg(args, unsigned long long)
                        : (long_count == 1) ? va_arg(args, unsigned long)
                        : va_arg(args, unsigned int);
            utoa64(v, tmp, 10, 0);
            break;
        }

        case 'x': {
            uint64_t v = (long_count == 2) ? va_arg(args, unsigned long long)
                        : (long_count == 1) ? va_arg(args, unsigned long)
                        : va_arg(args, unsigned int);
            utoa64(v, tmp, 16, 0);
            break;
        }

        case 'b': {
            uint64_t v = va_arg(args, uint64_t);
            utoa64(v, tmp, 2, 0);
            break;
        }

        case 'X': {
            uint64_t v = (long_count == 2) ? va_arg(args, unsigned long long)
                        : (long_count == 1) ? va_arg(args, unsigned long)
                        : va_arg(args, unsigned int);
            utoa64(v, tmp, 16, 1);
            break;
        }

        case 'p': {
            uint32_t v = (uint32_t)va_arg(args, void*);
            tmp[0] = '0';
            tmp[1] = 'x';
            utoa64(v, tmp + 2, 16, 0);
            break;
        }

        case 'f': {
            double d = va_arg(args, double);
            ftoa(d, tmp, precision < 0 ? 6 : precision);
            break;
        }

        case '%':
            tmp[0] = '%';
            tmp[1] = 0;
            break;

        default:
            buf_putc(buffer, size, &pos, *fmt);
            fmt++;
            continue;
        }

        out = (*tmp) ? tmp : out;

        size_t out_len = strlen(out);

        size_t str_limit = (size_t)-1;
        if (*fmt == 's' && precision >= 0)
            str_limit = precision;

        size_t visible_len = out_len;
        if (str_limit != (size_t)-1 && visible_len > str_limit)
            visible_len = str_limit;

        int pad = width - visible_len;
        if (pad < 0) pad = 0;

        char pad_char = (zero && !left) ? '0' : ' ';

        if (!left)
            while (pad-- > 0)
                buf_putc(buffer, size, &pos, pad_char);

        buf_puts(buffer, size, &pos, out, str_limit);

        if (left)
            while (pad-- > 0)
                buf_putc(buffer, size, &pos, ' ');

        fmt++;
    }

    if (pos < size)
        buffer[pos] = 0;
    else if (size > 0)
        buffer[size - 1] = 0;

    return pos;
}

int snprintf(char* buffer, size_t size, const char* fmt, ...) {
    va_list a; va_start(a, fmt);
    int r = vsnprintf(buffer, size, fmt, a);
    va_end(a);
    return r;
}

int sprintf(char* buffer, const char* fmt, ...) {
    va_list a; va_start(a, fmt);
    int r = vsnprintf(buffer, (size_t)-1, fmt, a);
    va_end(a);
    return r;
}

int vsprintf(char* buffer, const char* fmt, va_list a) {
    return vsnprintf(buffer, (size_t)-1, fmt, a);
}