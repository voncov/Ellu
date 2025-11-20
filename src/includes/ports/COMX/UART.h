/**
 * Заголовочный файл для UART-модуля последовательного ввода/вывода в COM1-COM8 порты
 */
#pragma once

#include <stdint.h>

#define SERIAL_PORT_COM1 0x3F8
#define SERIAL_PORT_COM2 0x2F8
#define SERIAL_PORT_COM3 0x3E8
#define SERIAL_PORT_COM4 0x2E8
#define SERIAL_PORT_COM5 0x5F8
#define SERIAL_PORT_COM6 0x4F8
#define SERIAL_PORT_COM7 0x5E8
#define SERIAL_PORT_COM8 0x4E8

typedef enum
{
    COM1 = SERIAL_PORT_COM1,
    COM2 = SERIAL_PORT_COM2,
    COM3 = SERIAL_PORT_COM3,
    COM4 = SERIAL_PORT_COM4,
    COM5 = SERIAL_PORT_COM5,
    COM6 = SERIAL_PORT_COM6,
    COM7 = SERIAL_PORT_COM7,
    COM8 = SERIAL_PORT_COM8
} COMX_PORT;

int32_t init_serial(COMX_PORT comx_port);
char read_serial(void);
void write_serial(char c);