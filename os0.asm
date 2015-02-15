    mov ah, 0x0e 
    mov al, 'F'
    int 0x10
loop:
    jmp loop

    times 510-($-$$)    db  0

    dw 0xaa55 ; magic bios number
