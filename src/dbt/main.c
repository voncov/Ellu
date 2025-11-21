/**
 * Dual Boot Technology
 * 
 * Модуль мультизагрузчика Ellu
 */
#include <stdio.h>
#include <cpu/procio.h>
#include <ports/COMX/UART.h>

void main(void) {
    PIC_remap(0x20, 0x28);
    COMX_PORT port = COM1;
    init_serial(port);
    IO_MODE mode = SERIAL;
    set_io_mode(mode);
    print("PIC remapped!");

    while (1);
}