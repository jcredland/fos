; void paging_enable(void)
; - sets the top bit of CR0 to enabling paging.  you must have loaded
;   paging addresses into CR3 using paging_set_address() first. 
global paging_enable
paging_enable:
    push ebp 
    mov ebp, esp
    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax
    pop ebp
    ret

; void paging_set_address(uint32 * table_ptr);
; - loads the paging address into CR3. 
global paging_set_address
paging_set_address:
    push ebp
    mov ebp, esp ; set-up the stack frame 
    ; load the address
    mov eax, [esp + 8]
    mov cr3, eax
    pop ebp
    ret

     
