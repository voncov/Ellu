/**
 * Начало ядра операционной системы Ellu
 */

#include <cpu/procio.h>

void main(void) {
    while (1) {}
    // Альтернативный метод, если вариант сверху падёт из-за break;
    halt();
}