
; switch_process(struct ProcessStructure * from, struct ProcessStructure * to)
;
; this switches stacks. 
;
; How?
global switch_process
switch_process:

    mov [eax], esp
    mov esp, edx
    
