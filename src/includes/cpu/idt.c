/**
 * Модуль-конфигуратор таблицы дескрипторов прерываний (IDT)
 * Источники с полезной информацией использовавшейся здесь:
 *      https://wiki.osdev.org/Interrupts_Tutorial
 *      https://wiki.osdev.org/Interrupt_Descriptor_Table
 */
#include "idt.h"
#include "procio.h"

__attribute__((aligned(0x10))) static idt_entry_t idt[32];
static idtr_t idtr;
static int vectors[IDT_MAX_DESCRIPTORS];
extern void* isr_stub_table[];

void idt_set_descriptor(uint8_t vector, void* isr, uint8_t flags) {
    idt_entry_t* descriptor = &idt[vector];

    descriptor->isr_lo         = (uint32_t)isr & 0xFFFF;
    descriptor->codeseg        = 0x08;
    descriptor->iattribs       = flags;
    descriptor->isr_hi         = (uint32_t)isr >> 16;
    descriptor->reserved       = 0;
}

void idt_init(void) {
    idtr.base = (uintptr_t)&idt[0];
    idtr.limit = (uint16_t)sizeof(idt_entry_t) * IDT_MAX_DESCRIPTORS - 1;

    memset((uint8_t*)&idt, 0, sizeof(idt_entry_t) * IDT_MAX_DESCRIPTORS);
    for (uint8_t vector = 0; vector < IDT_MAX_DESCRIPTORS; vector++) {
        idt_set_descriptor(vector, isr_stub_table[vector], 0x8E);
        vectors[vector] = 1;
    }

    __asm__ volatile ("lidt %0" : : "m"(idtr));
}