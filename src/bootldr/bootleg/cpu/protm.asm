; Простой модуль перехода в защищённый 32-битный режим
; Источник, взятый за пример: https://wiki.osdev.org/Protected_Mode

_KRNL_ADDR equ 0x10000

[bits 16]

jump_to_pm_msg db 0x0A, 0x0D, "Entering to protected mode...", 0x0

; Определяем `m_sw_32`
; Переходная функция в 32-битный защищённый режим
m_sw_32:
    cli

    mov si, jump_to_pm_msg
    call m_tty_print

    lgdt [gdt_descriptor]
    mov eax, cr0
    ; Устанавливаем в регистр eax значение 0x1
    or eax, 0x1
    ; Устанавливаем в регистр cr0 сам eax, мы включили защищённый режим процессора.
    mov cr0, eax
    jmp CODE_SEG:initpm32

[bits 32]
; Определяем `initpm32`
; Подробнее об это можно прочитать в https://wiki.osdev.org/Protected_Mode
; ! Это измененная версия того примера, они просто похожи !
initpm32:
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; mov esp, 0x90000

    jmp CODE_SEG:_KRNL_ADDR