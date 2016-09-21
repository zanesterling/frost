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
%include "stage2/A20.inc" ; helper functions for enabling the A20 line
%include "stage2/bootinfo.inc"
%include "stage2/common.inc" ; common definitions
%include "stage2/Fat12.inc" ; fat12 driver
%include "stage2/Floppy16.inc" ; floppy driver
%include "stage2/gdt.inc" ; global descriptor table stuff
%include "stage2/memory.inc" ; memory query functions


;*************************************************;
;   Data segment
;*************************************************;
section .data

LoadingMsg db "Preparing to load operating system...",13,10,0
MsgFailure db 0x0d, 0x0a, "ERROR: file kernel.sys not found. Press any key to reboot.", 0x0d, 0x0a, 0

boot_info:
istruc multiboot_info
	at multiboot_info.flags,			dd 0
	at multiboot_info.memoryLo,			dd 0
	at multiboot_info.memoryHi,			dd 0
	at multiboot_info.bootDevice,		dd 0
	at multiboot_info.cmdLine,			dd 0
	at multiboot_info.mods_count,		dd 0
	at multiboot_info.mods_addr,		dd 0
	at multiboot_info.syms0,			dd 0
	at multiboot_info.syms1,			dd 0
	at multiboot_info.syms2,			dd 0
	at multiboot_info.mmap_length,		dd 0
	at multiboot_info.mmap_addr,		dd 0
	at multiboot_info.drives_length,	dd 0
	at multiboot_info.drives_addr,		dd 0
	at multiboot_info.config_table,		dd 0
	at multiboot_info.bootloader_name,	dd 0
	at multiboot_info.apm_table,		dd 0
	at multiboot_info.vbe_control_info,	dd 0
	at multiboot_info.vbe_mode_info,	dw 0
	at multiboot_info.vbe_interface_seg,dw 0
	at multiboot_info.vbe_interface_off,dw 0
	at multiboot_info.vbe_interface_len,dw 0
iend

memory_map: resb MemoryMapEntry.size * 8

;*************************************************;
;   Second Stage Loader Entry Point
;************************************************;

section .text

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

        mov [boot_info+multiboot_info.bootDevice], dl

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
    ;   Get memory information
    ;********************************;
        mov dword [boot_info+multiboot_info.flags], 0xff

        xor eax, eax
        xor ebx, ebx
        call BiosGetBigMemorySize
        mov word [boot_info+multiboot_info.memoryLo], ax
        mov word [boot_info+multiboot_info.memoryHi], bx

        mov di, memory_map
        mov dword [boot_info+multiboot_info.mmap_addr], memory_map

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

MsgToKernel db "Jumping to kernel", 0x0a, 0

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
        mov esi, MsgToKernel
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

    cli
    mov eax, 0x2BADB002
    mov ebx, 0
    mov edx, dword [ImageSize]

    push dword boot_info
    push dword STOP; junk return address
    jmp CODE_DESC:IMAGE_PMODE_BASE

    STOP:
        cli
        hlt
