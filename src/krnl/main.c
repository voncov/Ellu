/**
 * Исходный: src/krnl/main.c
 * 
 * Главный файл ядра операционной системы Ellu
 */

#include <cpu/procio.h>

void main(void) {
    while (1) {}
    // Альтернативный метод, если вариант сверху падёт из-за break;
    halt();
}