%ifndef _ELF_INC_INCLUDED
%define _ELF_INC_INCLUDED

bits 32
section .data

ElfMagicNumber  db 0x7f, "ELF"
BinaryStart dd 0
BinaryEnd dd 0

; File header offsets
struc elf_fh
    .MAG0         resb 1
    .MAG1         resb 1
    .MAG2         resb 1
    .MAG3         resb 1
    .CLASS        resb 1
    .DATA         resb 1 ; 1 = little endian, 2 = big endian
    .VERSION      resb 1
    .OSABI        resb 1
    .PAD          resb 8
    .TYPE         resw 1
    .MACHINE      resw 1
    .VERSION2     resd 1
    .ENTRY        resd 1
    .PHOFF        resd 1
    .SHOFF        resd 1
    .FLAGS        resd 1
    .EHSIZE       resw 1
    .PHENTSIZE    resw 1
    .PHNUM        resw 1
    .SHENTSIZE    resw 1
    .SHNUM        resw 1
    .SHSTRNDX     resw 1
    .size:
endstruc

; Program header table offsets
struc elf_ph
    .TYPE          resd 1
    .OFFSET        resd 1
    .VADDR         resd 1
    .PADDR         resd 1
    .FILESZ        resd 1
    .MEMSZ         resd 1
    .FLAGS         resd 1
    .ALIGN         resd 1
    .size:
endstruc

; Section header table offsets
struc elf_sh
    .NAME          resd 1
    .TYPE          resd 1
    .FLAGS         resd 1
    .ADDRESS       resd 1
    .OFFSET        resd 1
    .SIZE          resd 1
    .PAD           resd 1
    .size:
endstruc

section .text

;*******************************************
; ElfCheckFileHeader()
;   in:ds:esi: pointer to file header
;   ret:eax: 1 if the header is bad, else 0
;*******************************************
ElfCheckFileHeader:
    ; check magic number
    mov bl, byte [esi+elf_fh.MAG0]
    cmp bl, 0x7f
    jne .error
    mov bl, byte [esi+elf_fh.MAG1]
    cmp bl, 'E'
    jne .error
    mov bl, byte [esi+elf_fh.MAG2]
    cmp bl, 'L'
    jne .error
    mov bl, byte [esi+elf_fh.MAG3]
    cmp bl, 'F'
    jne .error

    ; ensure machine architecture is x86
    mov bx, word [esi+elf_fh.MACHINE]
    mov ax, word [esi+elf_fh.TYPE]
    cmp ax, 1
    jne .big_endian

    cmp bx, 0x0300
    jne .error
    jmp .done
.big_endian:
    cmp bx, 0x03
    jne .error

.done:
    mov eax, 0
    ret
.error:
    mov eax, 1
    ret

;*******************************************
; ElfLoadBinary()
;   in:ds:esi: pointer to elf file
;   ret:eax: 0 if success, else:
;       - 1 if bad file header
;*******************************************
ElfLoadBinary:
    push esi

    ; check the file header
    call ElfCheckFileHeader
    test eax, eax
    jz .cont
    mov eax, 1
    jmp .error
    .cont:

    ; grab program header table memory location
    mov eax, esi
    add eax, dword [esi+elf_fh.PHOFF]
    push eax

    ; read .text program header
    add esi, dword [eax+elf_ph.OFFSET]
    mov edi, dword [eax+elf_ph.VADDR]
    mov [BinaryStart], edi
    mov ecx, dword [eax+elf_ph.FILESZ]
    cld
    rep movsd

    ; read .data/.bss program header
    pop eax
    add eax, elf_ph.size
    mov edi, dword [eax+elf_ph.VADDR]
    add edi, dword [eax+elf_ph.MEMSZ]
    mov [BinaryEnd], edi
    mov edi, dword [eax+elf_ph.VADDR]
    pop esi
    add esi, dword [eax+elf_ph.OFFSET]
    mov ecx, dword [eax+elf_ph.FILESZ]
    cld
    rep movsd

    mov ecx, [BinaryEnd]
    sub ecx, [BinaryStart]

.done:
    mov ax, 0
    ret
.error:
    ret

%endif
