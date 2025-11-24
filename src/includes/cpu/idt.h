/**
 * Заголовочный файл модуля-конфигуратора таблицы дескрипторов прерываний (IDT)
 */
#pragma once
#include <stdint.h>
#include <string.h>
#define IDT_MAX_DESCRIPTORS 48

typedef struct
{
    uint16_t isr_lo;
    uint16_t codeseg;
    uint8_t reserved;
    uint8_t iattribs;
    uint16_t isr_hi;
} __attribute__((packed)) idt_entry_t;

typedef struct
{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) idtr_t;

void idt_set_descriptor(uint8_t vector, void* isr, uint8_t flags);
void idt_init(void);