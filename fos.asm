;[org 0x7c00]
    global main
    bits 16

main:
; configure stack and segment registers
    mov bp, 0x8000
    mov sp, bp
; store disk information
    mov [drive], dl
; print string
    mov bx, string_loading
    call print_string
; load OS
    mov ah, 02h
    mov bx, main_os_code
    call load_fos
    jmp main_os_code

endless_loop:
    jmp endless_loop

;loop:
;#    jmp loop

;   BX: address of zero terminated string to print
print_string:
    mov ah, 0x0e
    mov al, [bx]
    cmp al, 0
    jz end_print_string
    int 0x10
    inc bx
    jmp print_string
end_print_string:
    ret 

; AL contains the number of sectors to read
load_fos:
    mov al, 2h ; number of sectors
    mov ch, 0h ; cylinder
    mov dh, 0h ; head
    mov cl, 2h ; sector 2
    int 13h
    jc disk_error

    ret
disk_error:
    mov bx, disk_error_string
    call print_string
    jmp $

;f-os strings
string_loading:
    db 'F/OS v0.01 loading...', 0
disk_error_string:
    db 'disk read error.', 0

drive:
    db 0


    times 510-($-$$)    db  0

    dw 0xaa55 ; magic bios number
; -------------------------------------------------------------------------------
; END BOOT LOADER
; -------------------------------------------------------------------------------
main_os_code:
    mov bx, str_main_os_loaded
    call print_string
    jmp $
    
str_main_os_loaded:
    db 'OS Loaded', 0

space:
    times 2048-($-$$) db 0  
