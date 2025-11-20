/**
 * Dual Boot Technology
 * 
 * Модуль мультизагрузчика Ellu
 */
#include <stdio.h>
#include <cpu/procio.h>
#include <ports/COMX/UART.h>

void main(void) {
    COMX_PORT port = COM1;
    init_serial(port);
    IO_MODE mode = SERIAL;
    set_io_mode(mode);
    print("Hello, World!");
    while (1);
    // Альтернативный метод, если вариант сверху падёт из-за break;
    halt();
}