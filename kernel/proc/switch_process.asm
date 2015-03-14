;
; void switch_process(SavedKernelContext * new_stack_ptr, SavedKernelContext ** save_old_one_here)
;
; This switches stacks.  It's a magic trick.
;
global switch_kernel_context
switch_kernel_context
    mov edx, [esp + 4]
    mov eax, [esp + 8]

    push edi
    push esi
    push ebx
    push ebp

    ; TODO - save floating point state (and other stuff?) or maybe we do this 
    ; in the interrupt return

    mov [eax], esp
    mov esp, edx
   
    pop ebp
    pop ebx
    pop esi
    pop edi
    
    ; And come out of a different rabbit hole 
    ret 
