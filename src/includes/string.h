/**
 * Заголовочный вспомогательный файл для libc-модуля `string`
 */
#pragma once
#include "stdarg.h"
#include "stdint.h"
int vsnprintf(char* buffer, size_t size, const char* fmt, va_list args);
int snprintf(char* buffer, size_t size, const char* fmt, ...);
int sprintf(char* buffer, const char* fmt, ...);
int vsprintf(char* buffer, const char* fmt, va_list a);