
;
; Memory detection routines. 
;
; Contacts the BIOS and asks for a memory map.  Stores it for 
; later use by the kernel. 
; 
; ES should point to our data segment. Probably 0 at this stage. 
;
global bios_detect_ram
bios_detect_ram:
    pusha

    xor bp, bp
    xor ebx, ebx
    mov di, ram_data
    call .callint
    jc .error
    mov edx, 'PAMS' ; some magic number required SMAP
    cmp eax, edx
    jne .error
    test ebx, ebx
    je .error ; if ebx is zero we didn't read anything
    jmp .more
.next
    call .callint
.more
    add di, 24
    inc bp
    cmp di, ram_data_end
;    jge .error
    cmp ebx, 0
    jne .next
    mov [ram_data_size], bp
    jmp .done

.error
    mov word [ram_data_size], 0xFFFF
    stc
.done

    popa
    ret

.callint
    mov [es:di + 20], dword 1
    mov eax, 0xe820 ; function call
    mov edx, 'PAMS' ; some magic number required SMAP
    mov ecx, 24 ; size of structure (ram_data)
    int 15h
    ret


global ram_data_size
global ram_data 

ram_data_size:
    dw  0
ram_data: 
    times 512 db 0
ram_data_end:

