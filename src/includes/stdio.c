/**
 * libc-модуль ввода/вывода
 */
#include "stdio.h"

static IO_MODE curr = SERIAL;

/**
 * ### print
 * 
 * Вывод строки
 */
void print(const char* str)
{
    switch (curr)
    {
    case VGA:
        /* code */
        break;
    case SERIAL:
        while (*str) write_serial(*str++);
        break;
    default:
        break;
    }
}

/**
 * ### printf
 * 
 * Отформатированный вывод строки
 */
void printf(const char* fmt, ...)
{
    char buf[0x2000];
    va_list ap;
    va_start(ap, fmt);
    vsprintf(buf, fmt, ap);
    va_end(ap);
    print(buf);
}

/**
 * ### set_io_mode
 * 
 * Задать режим вывода
 */
void set_io_mode(IO_MODE value)
{
    curr = value;
}