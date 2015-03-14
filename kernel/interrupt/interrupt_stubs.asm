; Key service routines for the kernel
    [bits 32]

extern interrupt_handler
; ---------------------------------------------------
; INTERRUPT STUB ADDRESS TABLE
global interrupt_stub_vectors
interrupt_stub_vectors:
    %assign vcounter 0
    %rep 256
    dd interrupt_stub_ %+ vcounter
    %assign vcounter vcounter+1
    %endrep


; ---------------------------------------------------
; INTERRUPT STUB MACRO
;  32-bit standard interrupt handler stub with 
;  dummy variable added for interrupts that do not
;  generate an error code. 
;
    %macro handler 1
global interrupt_stub_%1
interrupt_stub_%1:
;    call debug_interrupt_hook
    %if (%1 = 8 || %1 = 10 || %1 = 11 || %1 = 12 || %1 = 13 || %1 = 14 || %1 = 17 || %1 = 30)
    %else
    push dword 0x0
    %endif
    push dword %1 ; trap number
    jmp interrupt_continue

%endmacro

; ---------------------------------------------------
; INTERRUPT STUB GENERATION MACRO
    %assign counter 0 
interrupt_handler_stubs:
    %rep 256
    handler counter
    %assign counter counter+1
    %endrep 


; ---------------------------------------------------
; INTERRUPT STUB COMMON CODE
interrupt_continue:
; create the rest of the stack frame (see InterruptStackFrame in interrupt/stack_frame.h). 
    push ds
    push es
    push fs
    push gs

    push eax
    push ecx
    push edx
    push ebx

    ;push esp
    push ebp
    push esi
    push edi

    push esp ; this puts the address of the structure 
             ; pushed to the stack as a parameter for interrupt_handler
    call interrupt_handler
    add esp, 4 ; and clean up.

; ----------------------------
; INTERRUPT RETURN CODE
; general interrupt return code is called directly when a new process
; is added.  See ProcessManager and Process.

global interrupt_return
interrupt_return:

    pop edi
    pop esi
    pop ebp
    ;pop esp

    pop ebx
    pop edx
    pop ecx
    pop eax

    pop gs
    pop fs
    pop es
    pop ds

    add esp, 8 ; remove the errorcode and trap number

    iret

interrupt_vector_table:

