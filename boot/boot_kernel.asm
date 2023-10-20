[bits 16]
boot_start:
    call print_real_mode_nl
    call set_print_real_mode_cursor

    mov ebx, 0       ; ebx=0，ebx
    mov ax, 0x00
    mov es, ax
    mov di, 0x8000

Label_Get_Mem_Struct:
    mov eax, 0x0E820         ; 功能号 ax=E820h 从硬件查询系统地址映射信息
    mov ecx, 24              ; struct 的大小
    mov edx, 0x534D4150      ; 这是一个魔法数字，即字符串“SMAP”
    ; mov [es:di + 20], dword 1
    int 15h
    jc  Label_Get_Mem_Fail   ; 读取成功时 CF=0，继续向下执行，否则跳转到错误分支
    add di, 24               ; 读取下一个 struct

    cmp ebx,  0
    jne Label_Get_Mem_Struct  ; ebx!=0，表示还未遍历完，继续遍历，否则继续向下执行
    jmp Label_Get_Mem_OK      ; 已遍历完，跳转到成功分支

Label_Get_Mem_OK:
    mov bx, MSG_BOOT_START
    call print_real_string

    call switch_long_mode
    jmp $

Label_Get_Mem_Fail:
    mov bx, MSG_MEM_ERROR
    call print_real_string
    jmp $

%include "boot/boot_print.asm"
%include "boot/boot_gdt.asm"
%include "boot/boot_long.asm"

MSG_BOOT_START db "Boot Start ", 0
MSG_MEM_ERROR db "Boot GET MEM ERROR", 0
