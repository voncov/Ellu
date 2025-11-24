/**
 * Модуль-помощник (ловитель) специальных прерываний-исключений от процессора
 * Такая процедура называется: Рутинное обслуживание прерываний (ISR)
 * Источники с полезной информацией использовавшейся здесь:
 *      https://wiki.osdev.org/Interrupts_Tutorial
 *      https://wiki.osdev.org/Interrupt_Service_Routines
 */
#include <stdio.h>
#include "procio.h"

typedef struct __attribute__((packed)) cpu_state {
    uint32_t gs;
    uint32_t fs;
    uint32_t es;
    uint32_t ds;
    
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    
    uint32_t int_no;
    uint32_t error_code;
    
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
    uint32_t user_esp;
    uint32_t ss;
} cpu_state_t;

static const char* exception_names[] = {
    "Division By Zero", "Debug", "Non-Maskable Interrupt", "Breakpoint",
    "Into Detected Overflow", "Out of Bounds", "Invalid Opcode", "No Coprocessor",
    "Double Fault", "Coprocessor Segment Overrun", "Bad TSS", "Segment Not Present",
    "Stack Fault", "General Protection Fault", "Page Fault", "Unknown Interrupt",
    "Coprocessor Fault", "Alignment Check", "Machine Check", "SIMD Floating-Point Exception",
    "Virtualization Exception", "Reserved", "Reserved", "Reserved",
    "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved",
    "Security Exception", "Reserved"
};

__attribute__((noreturn)) 
void exception_handler(cpu_state_t *state);

void exception_handler(cpu_state_t *state) {
    printf("\n=== CPU Exception #%d ===\n", state->int_no);
    printf("EAX=%08X EBX=%08X ECX=%08X EDX=%08X\n", state->eax, state->ebx, state->ecx, state->edx);
    printf("ESI=%08X EDI=%08X EBP=%08X\n", state->esi, state->edi, state->ebp);
    printf("\nSegment Registers:\n");
    printf("DS=%04X ES=%04X FS=%04X GS=%04X\n", state->ds, state->es, state->fs, state->gs);
    printf("\nException Details:\n");
    printf("Interrupt: %d, Error Code: %08X\n", state->int_no, state->error_code);
    printf("EIP=%08X CS=%04X EFLAGS=%08X\n", state->eip, state->cs, state->eflags);
    printf("\nStack Information:\n");
    printf("User ESP=%08X SS=%04X\n", state->user_esp, state->ss);
    if (state->int_no < 32) {
        printf("Exception: %s\n", exception_names[state->int_no]);
    }
    
    printf("System Halted.\n");
    
    while(1) {
        __asm__ __volatile__("cli; hlt");
    }
}