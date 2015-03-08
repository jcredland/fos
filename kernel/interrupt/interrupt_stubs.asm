; Key service routines for the kernel
    [bits 32]

extern interrupt_handler

; 32-bit standard interrupt handler stub
; Interrupt handler macro
%macro handler 1
global interrupt_stub_%1
interrupt_stub_%1:
    pusha
    push dword 0
    push dword %1

    call interrupt_handler

    pop eax
    pop eax
    popa

    iret
%endmacro

    %assign counter 0 
interrupt_handler_stubs:
    %rep 256
    handler counter
    %assign counter counter+1
    %endrep 
