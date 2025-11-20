/**
 * UART-модуль последовательного ввода/вывода в COM1-COM8 порты
 * 
 * Источники, взятые за пример:
 *      https://wiki.osdev.org/Serial_Ports
 */
#include "UART.h"
#include <cpu/procio.h>

static COMX_PORT current_port = COM1;

/**
 * ### init_serial
 * 
 * Проинициализировать последовательный порт
 */
int32_t init_serial(COMX_PORT comx_port)
{
    uint8_t test_char = 0xAE;
    // Включить прерывания у порта
    outb((uint16_t)comx_port + 1, (uint8_t)0x00);
    // Включить DLAB (Divisor Latch Access Bit || Бит доступа к Блокировке делителя)
    outb((uint16_t)comx_port + 3, (uint8_t)0x80);
    io_wait();
    outb((uint16_t)comx_port + 0, (uint8_t)0x03); // Включить делитель на 3 (нижний байт)
    outb((uint16_t)comx_port + 1, (uint8_t)0x00); //                        (высший байт)
    outb((uint16_t)comx_port + 3, (uint8_t)0x03); // 8 бит, без четности, один стоп-бит
    outb((uint16_t)comx_port + 2, (uint8_t)0xC7); // Включить FIFO, очистить их с 14-байтовым порогом
    outb((uint16_t)comx_port + 4, (uint8_t)0x0B); // IRQs включен, RTS/DSR установлен
    // Установить в режим обратной связи, чтобы тестировать последовательный чип
    outb((uint16_t)comx_port + 4, (uint8_t)0x1E);
    io_wait();
    // Тестируем последовательный чип (отправляем байт 0xAE и смотрим что вернёт последовательный порт)
    outb((uint16_t)comx_port + 0, (uint8_t)test_char);
    
    // Проверяем, не исправен ли последовательный порт
    uint8_t test_result = inb((uint16_t)comx_port + 0);
    if (test_result != test_char) {
        return 1;
    }
    io_wait();
    // Если последовательный порт исправен, перевести его в нормальный режим работы
    // (без обратной связи с включенными IRQS и включенными битами OUT#1 и OUT#2)
    outb((uint16_t)comx_port + 4, (uint8_t)0x0F);
    current_port = comx_port;
    return 0;
}

/**
 * ### serial_received
 * 
 * Проверить переданные данные в последовательный порт
 */
static int32_t serial_received(void)
{
    uint8_t result = inb((uint16_t)current_port + 5);
    io_wait();
    // Возвращаем 0 или 1 если result нечетный или четный
    return (int32_t)result & 1;
}

/**
 * ### is_transmit_empty
 * 
 * Проверить, пустой ли передатчик
 */
static int32_t is_transmit_empty(void)
{
    uint8_t result = inb((uint16_t)current_port + 5);
    // Возвращаем шестой бит result
    return (int32_t)result & 0x20;
}

/**
 * ### read_serial
 * 
 * Прочитать значение из последовательного порта
 */
char read_serial(void)
{
    while (serial_received() == 0);
    // Читать из порта значение
    char result = (char)inb(current_port);
    return result;
}


/**
 * ### write_serial
 * 
 * Запись символа в последовательный порт
 */
void write_serial(char c)
{
    while (is_transmit_empty() == 0);
    // Записываем в порт значение
    outb((uint16_t)current_port, (uint8_t)c);
}