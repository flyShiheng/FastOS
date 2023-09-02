; gdt in Protected Mode 
gdt_start:
    ; the GDT starts with a null 8-byte
    dd 0x0 ; 4 byte
    dd 0x0 ; 4 byte

; GDT for code segment. base = 0x00000000, length = 0xfffff
; for flags, refer to os-dev.pdf document, page 36
gdt_code:
    dw 0xffff    ; segment length, bits 0-15
    dw 0x0       ; segment base, bits 0-15
    db 0x0       ; segment base, bits 16-23
    db 10011010b ; flags (8 bits)
    db 11001111b ; flags (4 bits) + segment length, bits 16-19
    db 0x0       ; segment base, bits 24-31

; GDT for data segment. base and length identical to code segment
; some flags changed, again, refer to os-dev.pdf
gdt_data:
    dw 0xffff
    dw 0x0
    db 0x0
    db 10010010b
    db 11001111b
    db 0x0

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1 ; size (16 bit), always one less of its true size
    dd gdt_start ; address (32 bit)

; define some constants for later use
CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start


; GDP in Long Mode 
GDT_start:
    dq 0x0000000000000000              ; Null Descriptor - should be present.
GDT_code:
    dq 0x00209A0000000000              ; 64-bit code descriptor (exec/read).
GDT_data:
    dq 0x0000920000000000              ; 64-bit data descriptor (read/write).
ALIGN 4
    dw 0                               ; Padding to make the "address of the GDT" field aligned on a 4-byte boundary
GDT_descriptor:
    dw $ - GDT_start - 1               ; 16-bit Size (Limit) of GDT.
    dd GDT_start                       ; 32-bit Base Address of GDT. (CPU will zero extend to 64-bit)

LONG_CODE_SEG equ GDT_code - GDT_start
LONG_DATA_SEG equ GDT_data - GDT_start
