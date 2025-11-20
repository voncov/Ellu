/**
 * Dual Boot Technology
 * Исходный: src/dualboot/main.c
 * 
 * Главный файл мультизагрузчика Ellu
 */

#include <stdint.h>
#include <cpu/procio.h>

void main(void) {
    while (1) {}
    // Альтернативный метод, если вариант сверху падёт из-за break;
    halt();
}