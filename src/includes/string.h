/**
 * Путь: src/includes/string.h
 * 
 * Заголовочный вспомогательный файл для модуля `string`
 * 
 * Изменения:
 *      16 Nov, 11:34 :: Файл создан
 *      16 Nov, 13:36 :: Добавлены функции форматирования строки.
 */
#pragma once
#include <stdarg.h>
int vsnprintf(char* buffer, size_t size, const char* fmt, va_list args);
int snprintf(char* buffer, size_t size, const char* fmt, ...);
int sprintf(char* buffer, const char* fmt, ...);
int vsprintf(char* buffer, const char* fmt, va_list a);