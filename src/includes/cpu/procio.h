/**
 * Заголовочный файл модуля управления портами процессора Intel x86_x64
 * 
 * Источники с полезной информацией использовавшейся здесь:
 *      https://wiki.osdev.org/Inline_Assembly/Examples
 *      https://wiki.osdev.org/Shutdown
 *      https://wiki.osdev.org/8259_PIC
 */
#pragma once

#include "../stdint.h"

#define PIC1 0x20               // Порт главного PIC
#define PIC2 0xA0               // Порт ведомого PIC
#define PIC1_CMD PIC1
#define PIC1_DATA (PIC1 + 1)
#define PIC2_CMD PIC2
#define PIC2_DATA (PIC2 + 1)
#define PIC_EOI 0x20            // Значение для завершения обработки прерывания (IRQ)

#define ICW1_ICW4 0x01		    // Указывает на то, что ICW4 будет присутствовать
#define ICW1_SINGLE	0x02		// Одиночный (каскадный) режим
#define ICW1_INTERVAL4 0x04		// Интервал между вызовами 4 (8)
#define ICW1_LEVEL 0x08		    // Режим срабатывания уровня (пограничный)
#define ICW1_INIT 0x10          // Инициализация (обязательно)

#define ICW4_8086 0x01		    // Режим 8086/88 (MCS-80/85)
#define ICW4_AUTO 0x02		    // Автоматическое (обычное) завршение обработки прерывания 
#define ICW4_BUF_SLAVE 0x08		// Буферизованный режим/ведомый
#define ICW4_BUF_MASTER	0x0C	// Буферизованный режим/главный
#define ICW4_SFNM 0x10		    // Специальный полностью вложенный (нет)

#define CASCADE_IRQ 2

/**
 * ### halt
 * 
 * Зависание процессора до перезагрузки (нажатия Ctrl+Alt+Del)
 */
static inline void halt(void)
{
    __asm__ volatile("hlt");
}

/**
 * ### interrupts_enable
 * 
 * Включить прерывания процессора
 */
static inline void interrupts_enable(void)
{
    __asm__ volatile("sti");
}

/**
 * ### interrupts_disable
 * 
 * Выключить прерывания процессора
 */
static inline void interrupts_disable(void)
{
    __asm__ volatile("cli");
}

/**
 * ### outb
 * 
 * Отправить 8-разрядное значение в порт ввода/вывода
 * @param port Порт ввода/вывода
 * @param val Значение помещаемое в порт
 */
static inline void outb(uint16_t port, uint8_t val)
{
    __asm__ volatile ("outb %b0, %w1" : : "a"(val), "Nd"(port) : "memory");
}

/**
 * ### outw
 * 
 * Отправить 16-разрядное значение в порт ввода/вывода
 * @param port Порт ввода/вывода
 * @param val Значение помещаемое в порт
 */
static inline void outw(uint16_t port, uint16_t val)
{
    __asm__ volatile ("outw %w0, %w1" : : "a"(val), "Nd"(port) : "memory");
}

/**
 * ### outl
 * 
 * Отправить 32-разрядное значение в порт ввода/вывода
 * @param port Порт ввода/вывода
 * @param val Значение помещаемое в порт
 */
static inline void outl(uint16_t port, uint32_t val)
{
    __asm__ volatile ("outl %0, %w1" : : "a"(val), "Nd"(port) : "memory");
}

/**
 * ### inb
 * 
 * Получить 8-разрядное значение из порта ввода/вывода.
 * @param port Порт ввода/вывода
 * @returns 8-разрядное значение из порта ввода/вывода
 */
static inline uint8_t inb(uint16_t port)
{
    uint8_t ret;
    __asm__ volatile("inb %w1, %b0"
                    : "=a" (ret)
                    : "Nd" (port)
                    : "memory"
                    );
    return ret;
}

/**
 * ### inw
 * 
 * Получить 16-разрядное значение из порта ввода/вывода.
 * @param port Порт ввода/вывода
 * @returns 16-разрядное значение из порта ввода/вывода
 */
static inline uint16_t inw(uint16_t port)
{
    uint16_t ret;
    __asm__ volatile("inw $w1, %w0"
                    : "=a" (ret)
                    : "Nd" (port)
                    : "memory"
                    );
    return ret;
}

/**
 * ### inl
 * 
 * Получить 32-разрядное значение из порта ввода/вывода.
 * @param port Порт ввода/вывода
 * @returns 32-разрядное значение из порта ввода/вывода
 */
static inline uint32_t inl(uint16_t port)
{
    uint32_t ret;
    __asm__ volatile("inw $w1, %0"
                    : "=a" (ret)
                    : "Nd" (port)
                    : "memory"
                    );
    return ret;
}

/**
 * ### io_wait
 * 
 * Ожидать в течение очень короткого промежутка времени (обычно от 1 до 4 микросекунд).
 */
static inline void io_wait(void)
{
    outb(0x80, 0);
}

/**
 * ### shutdown
 * 
 * Выключить машину
 */
static inline void shutdown(void)
{
    // Для Bochs, и старых версий QEMU
    outw(0xB004, 0x2000);
    // Для новых версий QEMU
    outw(0x604, 0x2000);
    // Для VirtualBox
    outw(0x4004, 0x3400);
    // Для облачных ВМ
    outw(0x600, 0x34);
    // если всё равно так ничего и не произошло
    interrupts_disable();
    halt();
}

/**
 * ### cpu_reset
 * 
 * Сброс процессора
 */
static inline void cpu_reset(void)
{
    uint64_t null_idtr = 0;
    __asm__ __volatile__("lidt %0; int3" :: "m" (null_idtr));
    // shutdown если ничего так и не произошло
    shutdown();
}

/**
 * ### PIC_sendEOI
 * 
 * Отправка в PIC завершения обработки прерывания (IRQ)
 * @param irq Номер прерывания
 */
static inline void PIC_sendEOI(uint8_t irq)
{
    if (irq >= 0x08)
    {
        outb((uint16_t)PIC2_CMD, (uint8_t)PIC_EOI);
    }
    outb((uint16_t)PIC1_CMD, (uint8_t)PIC_EOI);
}

/**
 * ### PIC_remap
 * 
 * Переназначение программируемого контроллера прерываний
 * @param offset1 Смещение вектора основного PIC
 * @param offset2 Смещение вектора ведомого PIC
 */
static inline void PIC_remap(int32_t offset1, int32_t offset2)
{
	outb((uint16_t)PIC1_CMD, (uint8_t)(ICW1_INIT | ICW1_ICW4));     // запускает последовательность инициализации (в каскадном режиме)
	io_wait();
	outb((uint16_t)PIC2_CMD, (uint8_t)(ICW1_INIT | ICW1_ICW4));
	io_wait();
	outb((uint16_t)PIC1_DATA, (uint8_t)offset1);                   // ICW2: Смещение вектора основного PIC
	io_wait();
	outb((uint16_t)PIC2_DATA, (uint8_t)offset2);                   // ICW2: Смещение вектора ведомого PIC
	io_wait();
	outb((uint16_t)PIC1_DATA, (uint8_t)(1 << CASCADE_IRQ));        // ICW3: Сообщить главному PIC, что в IRQ2 есть подчиненный PIC.
	io_wait();
	outb((uint16_t)PIC2_DATA, (uint8_t)0x02);                      // ICW3: Сообщить подчиненному PIC его каскадный идентификатор (0000 0010).
	io_wait();
	
	outb((uint16_t)PIC1_DATA, (uint8_t)ICW4_8086);                 // ICW4: попросите обеих PIC использовать режим 8086 (а не режим 8080)
	io_wait();
	outb((uint16_t)PIC2_DATA, (uint8_t)ICW4_8086);
	io_wait();

	// Снимите маску с обеих PIC.
	outb((uint16_t)PIC1_DATA, (uint8_t)0x00);
	outb((uint16_t)PIC2_DATA, (uint8_t)0x00);
}

/**
 * ### PIC_disable
 * 
 * Отключение программируемого контроллера прерываний
 */
static inline void PIC_disable(void)
{
    outb((uint16_t)PIC1_DATA, (uint8_t)0xFF);
    outb((uint16_t)PIC2_DATA, (uint8_t)0xFF);
}