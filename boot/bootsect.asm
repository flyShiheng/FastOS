[org 0x7c00]
start:
    mov bp, 0x9000
    mov sp, bp
    mov bx, MSG_REAL_MODE
    call print_real_string
    call print_real_mode_nl
    jmp $

print_real_string:
    pusha
print_start:
    mov al, [bx] ; bx中保存着数据的首地址
    cmp al, 0    ; 为空就返回
    je print_done
    mov ah, 0x0e  ; 显示char字符
    int 0x10
    add bx, 1
    jmp print_start
print_done:
    popa
    ret

print_real_mode_nl:
    pusha
    mov ah, 0x0e
    mov al, 0x0a   ; ASCII 0x0a = 10 = LF 换行
    int 0x10
    popa
    ret

MSG_REAL_MODE db "Started in 16-bit Real Mode", 0

times 510 - ($ - $$) db 0
dw 0xaa55
