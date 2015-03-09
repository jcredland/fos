;
; void switch_process(const uint32 stack_to_load, uint32 * stack_ptr_save_location)
;
; This switches stacks.  It's a magic trick.
;
global switch_process
switch_process:
    push eax
    push ebx
    push ecx
    push edx

    push edi
    push esi
    push ebp
    pushf 

    ; TODO - save floating point state (and other stuff?)

    mov eax, [esp + 12]
    mov edx, [esp + 8]
    ; Pop a pill
    mov [eax], esp
    mov esp, edx
   
    popf
    pop ebp
    pop esi
    pop edi
    
    pop edx
    pop ecx
    pop ebx
    pop eax
   
    ; And come out of a different rabbit hole 
    ret 
