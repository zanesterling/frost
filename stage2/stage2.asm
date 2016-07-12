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

%include "stdio.inc" ; basic io routines
%include "gdt.inc" ; global descriptor table stuff


;*************************************************;
;   Data segment
;*************************************************;

LoadingMsg  db  "Preparing to load operating system...",13,10,0


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
    ;   Enter protected mode
    ;********************************;
        cli
        mov eax, cr0
        or eax, 1   ; set pmode bit
        mov cr0, eax
        jmp 08h:Stage3 ; far jump to fix CS


;*************************************************;
;   Stage 3 entry point
;*************************************************;

bits 32

Stage3:

    ;********************************;
    ;   Set up registers for pmode
    ;********************************;
        mov ax, 0x10    ; set data segments to use data selector from gdt
        mov ds, ax
        mov ss, ax
        mov es, ax
        mov esp, 90000h ; TODO shouldn't this be 0x90000?

    STOP:
        cli
        hlt
