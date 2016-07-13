;*************************************************
;   stdio.inc
;       -Input/Output routines
;
;   OS Development Series
;*************************************************

%ifndef __STDIO_INC_67343546FDCC56AAB872_INCLUDED__
%define __STDIO_INC_67343546FDCC56AAB872_INCLUDED__

;************************************************;
;   Puts16 ()
;       -Prints a null terminated string
;   DS=>SI: 0 terminated string
;************************************************;

bits    16

Puts16:
        pusha               ; save registers
    .Loop1:
        lodsb               ; load next byte from string from SI to AL
        or  al, al          ; Does AL=0?
        jz  Puts16Done      ; Yep, null terminator found-bail out
        mov ah, 0eh         ; Nope-Print the character
        int 10h         ; invoke BIOS
        jmp .Loop1          ; Repeat until null terminator found
    Puts16Done:
        popa                ; restore registers
        ret             ; we are done, so return


;************************************************;
;   32-bit functions
;************************************************;

bits 32

%define     VIDMEM  0xB8000
%define     COLS    80
%define     LINES   25
%define     CHAR_ATTRIB 15

;************************************************;
;   Data section
;************************************************;
_CurX   db 0
_CurY   db 0


;************************************************;
;   Putch32()
;       - Prints a character to the screen
;   BL => Character to print
;************************************************;

Putch32:
        pusha

    ;********************************;
    ;    Get destination for char
    ;********************************;
        ; compute y offset
        xor eax, eax
        mov al, byte [_CurY]
        mov ecx, COLS * 2
        mul ecx
        push eax

        ; compute x offset
        xor eax, eax
        mov al, byte [_CurX]
        mov cl, 2
        mul cl
        pop ecx
        add eax, ecx

        ; set edi to final destination
        mov edi, VIDMEM
        add edi, eax

    ;********************************;
    ;   Print character
    ;********************************;
        ; check for newline
        cmp bl, 0x0a
        je  .Row

        ; print the character
        mov dl, bl
        mov dh, CHAR_ATTRIB
        mov word [edi], dx

        ; update next position
        inc byte [_CurX]
        cmp byte [_CurX], COLS
        jne .Done

    .Row:
        ; go to next row
        mov byte [_CurX], 0
        inc byte [_CurY]

    .Done:
        popa
        ret


;************************************************;
;   Puts32()
;       - Prints a string to the current location
;   ESI => null terminated string to print
;************************************************;

Puts32:
        pusha

    .loop:
        ; print a character
        mov bl, byte [esi]
        cmp bl, 0
        je  .done
        call Putch32

        inc esi
        jmp .loop

    .done:
        call MovCur

        popa
        ret

;************************************************;
;   MoveCur()
;       - Moves the hardware cursor to
;         _CurX, _CurY
;************************************************;

MovCur:
        pusha

        ; find memory offset of cursor
        xor eax, eax
        mov al, byte [_CurY]
        mov cl, COLS
        mul cl
        add al, byte [_CurX]
        mov ebx, eax

        ; store low byte
        mov al, 0x0f
        mov dx, 0x03d4
        out dx, al

        mov al, bl
        mov dx, 0x03d5
        out dx, al

        ; store high byte
        xor eax, eax
        mov al, 0x0e
        mov dx, 0x03d4
        out dx, al

        mov al, bh
        mov dx, 0x03d5
        out dx, al

        popa
        ret


;************************************************;
;   ClearScr32()
;       - Clears the screen
;************************************************;
ClearScr32:
        pusha
        cld
        mov edi, VIDMEM
        mov cx, 2000
        mov ah, CHAR_ATTRIB
        mov al, ' '
    rep stosw

        mov byte [_CurX], 0
        mov byte [_CurY], 0
        popa
        ret

%endif ;__STDIO_INC_67343546FDCC56AAB872_INCLUDED__
