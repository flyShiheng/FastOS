[bits 16]
print_real_string:
    pusha
print_start:
    mov al, [bx]  ; bx中保存着数据的首地址
    cmp al, 0     ; 为空就返回
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
    mov al, 0x0a  ; ASCII 0x0a = 10 = LF 换行
    int 0x10
    popa
    ret

set_print_init:
    pusha
    mov ah, 0x00
    mov al, 0x03
    int 0x10
    popa
    ret

set_print_real_mode_page:
    pusha
    mov ah, 0x02
    mov bh, 0x0
    int 0x10
    popa
    ret

set_print_real_mode_cursor:
    pusha
    mov ah, 0x03
    mov bh, 0x0
    int 0x10

    mov ah, 0x02
    ; mov dh, 0x0  ; 行
    mov dl, 0  ; 列
    int 0x10
    popa
    ret

[bits 32]
print_pm:
    pusha
    mov edx, 0xb8000
print_pm_start:
    mov al, [ebx]
    mov ah, 0x0f  ; the color byte for each character
    cmp al, 0
    je print_pm_done
    mov [edx], ax  ; the show char
    add ebx, 1
    add edx, 2
    jmp print_pm_start
print_pm_done:
    popa
    ret
