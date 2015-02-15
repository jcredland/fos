    global main
    bits 16

main:
; configure stack. Segment registers presumed to be all 0.
    mov bp, stack.stacktop
    mov sp, bp
; store disk information
    mov [boot_drive], dl
; print string
    mov bx, string_loading
    call bios_print_string
; check a20 line
    ;call check_a20
    ;cmp ax, 0 ; zero means memory wraps around ...  really we need to handle this
    ;je a20_error
; load stage 2
    mov bx, stage2_start
    mov dl, [boot_drive]
    call load_stage_2
    jmp main_os_code

a20_error:
    mov bx, string_a20error
    call bios_print_string
    jmp $

; ---
; bios_print_string
; call: BX - address of zero terminated string to print
; clobbers: BX
; returns:
bios_print_string:
    push ax
.loop
    mov ah, 0x0e
    mov al, [bx]
    cmp al, 0
    jz .end
    int 0x10
    inc bx
    jmp .loop
.end
    pop ax
    ret 

; ---
; load_stage_2
; call: BX memory location to load to; DL drive to load from
; assumes stage 2 is immediately after the first sector.
load_stage_2:
    mov ah, 02h
    mov al, 8h ; number of sectors
    mov ch, 0h ; cylinder
    mov dh, 0h ; head
    mov cl, 2h ; sector 2
    int 13h
    jc .error

    ret
.error
    mov bx, disk_error_string
    call bios_print_string
    jmp $

; ---
; setup a20 address line

; Function: check_a20
;
; Purpose: to check the status of the a20 line in a completely self-contained state-preserving way.
;
; Returns: 0 in ax if the a20 line is disabled (memory wraps around)
;          1 in ax if the a20 line is enabled (memory does not wrap around)
 
check_a20:
    pushf
    push ds
    push es
 
    cli
 
    ; Set ES to the first segement, DS the last
    xor ax, ax 
    mov es, ax
    not ax ; ax = 0xFFFF
    mov ds, ax
 
    mov di, 0x0500
    mov si, 0x0510

    ; Store old contents of memory 
    mov al, byte [es:di]
    push ax
    mov al, byte [ds:si]
    push ax

    ; Perform check 
    mov byte [es:di], 0x00
    mov byte [ds:si], 0xFF
 
    cmp byte [es:di], 0xFF

    ; Restore old contents of memory 
    pop ax
    mov byte [ds:si], al
    pop ax
    mov byte [es:di], al
 
    mov ax, 0
    je .end
 
    mov ax, 1
 
.end    
    pop es
    pop ds
    popf
 
    ret


; ----- f-os strings
string_a20error:
    db 'A20 err.', 0
string_loading:
    db 'F/OS v0.01 loading...', 0
disk_error_string:
    db 'disk read error.', 0

boot_drive:
    db 0


    times 510-($-$$)    db  0

    dw 0xaa55 ; magic bios number

stage2_start:
stack:
    times 1024  db 0
.stacktop
    dd 0xdeadbeaf
; -------------------------------------------------------------------------------
; END BOOT LOADER
; -------------------------------------------------------------------------------
main_os_code:
    nop 
    nop
    mov bx, str_main_os_loaded
    call bios_print_string
    jmp protected_mode_start

str_main_os_loaded:
    db 'Stage 2 Loaded', 0

; Entering protected mode: 

protected_mode_start:
    cli
    lgdt [gdt_descriptor]
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    jmp 0x8:.next ; important moment.  clear the instruction pipe-line with a long jump

.next
    bits 32
    
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

hang:
    call vga_clear_screen
    mov bx, string.protectedmode
    call vga_print_string
    jmp $

;------------------------
; VGA DRIVER
;------------------------
    videomem_base equ 0xb8000
    videomem_end  equ 0xb8000 + 80 * 25
; ---
; vga_print_string
; call: BX - address of zero terminated string to print

vga_print_string:
    push ax
    push dx
    mov edx, videomem_base
    mov ah, 0xD
    mov ecx, [vgadata.cursor]
.loop
    mov al, [ebx]
    cmp al, 0
    jz .end

    mov [edx + ecx * 2], ax ; store to video memory
    inc ebx
    inc ecx

    cmp ecx, videomem_end
    je .scroll
    jmp .loop

.scroll
    sub ecx, [vgadata.columns]
    call vga_scroll_one_line
    jmp .loop

.end
    mov [vgadata.cursor], ecx
    pop dx
    pop ax
    ret

vga_scroll_one_line:
    push ecx
    push eax

    mov ecx, 0
    mov eax, 
    

    pop eax
    pop ecx 

    ret


vga_clear_screen:
    push ax
    push edi
    push ecx
    mov ah, 0
    mov al, ' '
    mov edi, videomem_base
    mov ecx, 25 * 80
    rep stosw

    mov ax, 0
    call vga_set_cursor_pos

    pop ecx
    pop edi
    pop ax
    ret


; Set and update the cursor location.  AH=x, AL=y
vga_set_cursor_pos:
;    mov [vgadata.cx], ah
;    mov [vgadata.cy], al

    push bx
    
    mov bh, ah
    mov bl, 80
    mul bl ; result in ax

    shr bx, 8
    add ax, bx

    mov [vgadata.cursor], ax

    call vga_update_cursor_pos
    pop bx
    ret

; sets the hardware cursor location based on the location
; in ah and al.
vga_update_cursor_pos:
    crt_index equ 0x03D4
    crt_data equ 0x03D5
    crt_idx_cursor_low equ 0xF
    crt_idx_cursor_high equ 0xE

    push bx
    mov bx, [vgadata.cursor]
     
    ; 1. transfer the low byte
    mov dx, crt_index 
    mov al, crt_idx_cursor_low
    out dx, al

    mov al, bl
    mov dx, crt_data
    out dx, al

    ; 2. transfer the high byte
    mov dx, crt_index 
    mov al, crt_idx_cursor_high
    out dx, al

    mov al, bh
    mov dx, crt_data
    out dx, al

    pop bx
    ret

vgadata:
.cursor
    dw 0 ; pointer based on cy * 80 + cx
.columns
    db 80
.rows
    db 25

string:
.protectedmode
    db 'F/OS running in 32-bit protected mode', 0
    db 'Pong to the Fong', 0
   
   
gdt:
.start
.null
    dw 0, 0, 0, 0
.code
    dw 0xffff ; limit
    dw 0 ; base
    db 0 ; base
    db 10011010b ; 1st flags , type flags
    db 11001111b ; 2nd flags , Limit ( bits 16 -19)
    db 0 ; base (high bits)
.data
    dw 0xffff ; limit
    dw 0 ; base
    db 0 ; base
    db 10010010b ; 1st flags , type flags
    db 11001111b ; 2nd flags , Limit ( bits 16 -19)
    db 0 ; base (high bits)
.end

gdt_descriptor:
.size
    dw gdt.end - gdt.start - 1
.addr
    dd gdt.start



space:
    times 8192-($-$$) db 0  
stage2_end:



