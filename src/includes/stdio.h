/**
 * Заголовочный файл libc-модуля ввода/вывода
 */
#pragma once
#include "string.h"
#include "stdarg.h"
#include "ports/COMX/UART.h"

typedef enum
{
    VGA,
    SERIAL
} IO_MODE;

void print(const char* str);
void printf(const char* fmt, ...);
void set_io_mode(IO_MODE value);