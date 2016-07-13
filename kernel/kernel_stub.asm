org     0x10000
bits    32

jmp Stage3

%include "util/stdio.inc"

msg db 0x0a, 0x0a, "Welcome to Kernel Land!", 0x0a, 0

Stage3:
        
        ; set registers
        mov ax, 0x10 ; set data segments to data selector
        mov ds, ax
        mov ss, ax
        mov es, ax
        mov esp, 90000h ; stack begins from 90000h

        ; clear screen and print success
        call ClearScr32
        mov ebx, msg
        call Puts32

        cli
        hlt
