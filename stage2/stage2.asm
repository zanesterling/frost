; Note: Here, we are executed like a normal
; COM program, but we are still in Ring 0.
; We will use this loader to set up 32 bit
; mode and basic exception handling

; This loaded program will be our 32 bit Kernel.

; We do not have the limitation of 512 bytes here,
; so we can add anything we want here!

bits 16     ; we are still in real mode

org 0x500   ; not sure about this

; we are loaded at linear address 0x10000

jmp main    ; jump to main


;*************************************************;
;   Preprocessor directives
;*************************************************;

%include "util/stdio.inc" ; basic io routines
%include "stage2/gdt.inc" ; global descriptor table stuff
%include "stage2/A20.inc" ; helper functions for enabling the A20 line
%include "stage2/Fat12.inc" ; fat12 driver
%include "stage2/Floppy16.inc" ; floppy driver
%include "stage2/common.inc" ; common definitions


;*************************************************;
;   Data segment
;*************************************************;

LoadingMsg db "Preparing to load operating system...",13,10,0
MsgFailure db 0x0d, 0x0a, "ERROR: file kernel.sys not found. Press any key to reboot.", 0x0d, 0x0a, 0


;*************************************************;
;   Second Stage Loader Entry Point
;************************************************;

main:

    ;********************************;
    ;   Set up segments and stack
    ;********************************;
        cli     ; clear interrupts
        xor ax, ax ; null the segments
        mov ds, ax
        mov es, ax
        mov ax, 0x9000  ; stack begins at 0x9000 - 0xffff
        mov ss, ax
        mov sp, 0xffff
        sti

    ;********************************;
    ;   Print loading message
    ;********************************;
        mov si, LoadingMsg
        call Puts16

    ;********************************;
    ;   Install the GDT
    ;********************************;
        call InstallGDT

    ;********************************;
    ;   Enable A20 line
    ;********************************;
        call EnableA20_KKbrd_Out

    ;********************************;
    ;   Print loading message
    ;********************************;
        mov si, LoadingMsg
        call Puts16

    ;********************************;
    ;   Initialize filesystem
    ;********************************;
        call LoadRoot

    ;********************************;
    ;   Load kernel
    ;********************************;
        ; set bp:bx to dest buffer
        mov bp, IMAGE_RMODE_BASE
        mov ebx, 0

        mov si, ImageName
        call LoadFile
        mov dword [ImageSize], ecx
        cmp ax, 0
        je EnterStage3

        ; failed to find kernel
        mov si, MsgFailure
        call Puts16

        ; wait for keypress, then reboot
        mov ah, 0
        int 0x16
        int 0x19
        cli ; should never get here
        hlt

    ;********************************;
    ;   Enter protected mode
    ;********************************;
    EnterStage3:
        cli
        mov eax, cr0
        or eax, 1   ; set pmode bit
        mov cr0, eax
        jmp CODE_DESC:Stage3 ; far jump to fix CS


;*************************************************;
;   Stage 3 entry point
;*************************************************;

bits 32

Stage3:

    ;********************************;
    ;   Set up registers for pmode
    ;********************************;
        mov ax, DATA_DESC
        mov ds, ax
        mov ss, ax
        mov es, ax
        mov esp, 0x90000

        call ClearScr32
        mov esi, LoadingMsg
        call Puts32

    ;********************************;
    ;   Copy kernel to 1mb
    ;********************************;
    CopyImage:
        mov eax, dword [ImageSize]
        movzx ebx, word [bpbBytesPerSector]
        mul ebx
        mov ebx, 4
        div ebx
        cld
        mov esi, IMAGE_RMODE_BASE
        mov edi, IMAGE_PMODE_BASE
        mov ecx, eax
    rep movsd

    ;********************************;
    ;   Execute kernel
    ;********************************;

    jmp CODE_DESC:IMAGE_PMODE_BASE

    STOP:
        cli
        hlt
