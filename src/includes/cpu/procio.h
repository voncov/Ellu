/**
 * Путь: src/multarch/i386/cpu/procio.h
 * 
 * Вспомогательный файл для управления портами процессора Intel x86_x64
 * 
 * Взято из https://wiki.osdev.org/Inline_Assembly/Examples
 * 
 * Изменения:
 *      13 Nov, 16:38 :: Файл создан.
 */
#pragma once

#include "../stdint.h"

/**
 * ### halt
 * 
 * Вызывает зависание процессора до перезагрузки (нажатия Ctrl+Alt+Del)
 */
static inline void halt(void)
{
    __asm__ volatile("hlt");
}

/**
 * ### interrupts_enable
 * 
 * Включает прерывания процессора
 */
static inline void interrupts_enable(void)
{
    __asm__ volatile("sti");
}

/**
 * ### interrupts_disable
 * 
 * Выключает прерывания процессора
 */
static inline void interrupts_disable(void)
{
    __asm__ volatile("cli");
}

/**
 * ### outb
 * 
 * Отправляет 8-разрядное значение в место ввода/вывода.
 */
static inline void outb(uint16_t port, uint8_t val)
{
    __asm__ volatile ("outb %b0, %w1" : : "a"(val), "Nd"(port) : "memory");
}

/**
 * ### outw
 * 
 * Отправляет 16-разрядное значение в место ввода/вывода.
 */
static inline void outw(uint16_t port, uint16_t val)
{
    __asm__ volatile ("outw %w0, %w1" : : "a"(val), "Nd"(port) : "memory");
}

/**
 * ### outl
 * 
 * Отправляет 32-разрядное значение в место ввода/вывода.
 */
static inline void outl(uint16_t port, uint32_t val)
{
    __asm__ volatile ("outl %0, %w1" : : "a"(val), "Nd"(port) : "memory");
}

/**
 * ### inb
 * 
 * Получает 8-разрядное значение из места ввода-вывода.
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
 * Получает 16-разрядное значение из места ввода-вывода.
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
 * Получает 32-разрядное значение из места ввода-вывода.
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
 * Ожидание в течение очень короткого промежутка времени (обычно от 1 до 4 микросекунд). Полезно для реализации небольшой задержки при переназначении PIC на старом оборудовании или в целом в качестве простого, но неточного ожидания. 
 */
static inline void io_wait(void)
{
    outb(0x80, 0);
}

/**
 * ### shutdown
 * 
 * Check https://wiki.osdev.org/Shutdown
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
 * Вызывает немедленную перезагрузку процессора, иначе зависание
 */
static inline void cpu_reset(void)
{
    uint64_t null_idtr = 0;
    __asm__ __volatile__("lidt %0; int3" :: "m" (null_idtr));
    // shutdown если ничего так и не произошло
    shutdown();
}