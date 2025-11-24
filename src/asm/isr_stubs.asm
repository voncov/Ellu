%macro isr_stub 2
isr_%1:
    cli
    push dword %2
    push dword %1
    jmp isr_common_stub
%endmacro
%macro isr_stub_noerr 1
isr_%1:
    cli
    push dword 0
    push dword %1
    jmp isr_common_stub
%endmacro

extern exception_handler

isr_stub_noerr 0
isr_stub_noerr 1
isr_stub_noerr 2
isr_stub_noerr 3
isr_stub_noerr 4
isr_stub_noerr 5
isr_stub_noerr 6
isr_stub_noerr 7
isr_stub 8, 1
isr_stub_noerr 9
isr_stub 10, 1
isr_stub 11, 1
isr_stub 12, 1
isr_stub 13, 1
isr_stub 14, 1
isr_stub_noerr 15
isr_stub_noerr 16
isr_stub 17, 1
isr_stub_noerr 18
isr_stub_noerr 19
isr_stub_noerr 20
isr_stub_noerr 21
isr_stub_noerr 22
isr_stub_noerr 23
isr_stub_noerr 24
isr_stub_noerr 25
isr_stub_noerr 26
isr_stub_noerr 27
isr_stub_noerr 28
isr_stub_noerr 29
isr_stub 30, 1
isr_stub_noerr 31

global isr_stub_table
isr_stub_table:
%assign i 0
%rep 32
    dd isr_%+i
%assign i i+1
%endrep

isr_common_stub:
    pusha
    
    push ds
    push es
    push fs
    push gs
    
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    push esp
    call exception_handler
    add esp, 4
    pop gs
    pop fs
    pop es
    pop ds
    popa
    add esp, 8
    iret