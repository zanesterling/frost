%ifndef __MEMORY_INC_INCLUDED__
%define __MEMORY_INC_INCLUDED__

bits 16

;*******************************************
; BiosGetMemorySize()
;   - Returns size of memory in KB, stored
;     in AL.
;   - If memory is larger than 64MB,
;     returns an invalid value.
;*******************************************

BiosGetMemorySize:
    int  0x12
    jc   .error
    test ax, ax
    je   .error
    test ah, 0x86
    je  .error
    test ah, 0x80
    je  .error
    ret
.error:
    mov ax, -1
    ret

;*******************************************
; BiosGetExtendedMemorySize()
;   - Returns number of KB in memory after
;     1MB.
;*******************************************

BiosGetExtendedMemorySize:
    mov  ax, 0x88
    int  0x15
    jc   .error
    test ax, ax
    je   .error
    cmp  ah, 0x86
    je   .error
    cmp  ah, 0x80
    je   .error
    ret
.error:
    mov ax, -1
    ret

;*******************************************
; BiosGetBigMemorySize
;   ret:ax: number of KB between 1 and 16MB
;   ret:bx: number of 64K blocks above 16MB
;   On error, ax=-1 and bx=0
;*******************************************

BiosGetBigMemorySize:
    push ecx
    push edx
    xor  ecx, ecx
    xor  edx, edx
    mov  ax, 0xe801
    int  0x15

    jc .error
    cmp  ah, 0x86
    je   .error
    cmp  ah, 0x80
    je   .error
    jcxz .use_ax
    mov  ax, cx
    mov  bx, dx

.use_ax:
    pop edx
    pop ecx
    ret

.error:
    mov ax, -1
    mov bx, 0
    pop edx
    pop ecx

;*******************************************
; BiosGetMemoryMap
;   in:es:di -> entry destination buffer
;*******************************************

struc MemoryMapEntry
    .baseAddress resq 1 ; base address of range
    .length      resq 1 ; length of range in bytes
    .type        resd 1 ; type of range
    .acpi_null   resd 1 ; reserved
    .size:
endstruc

BiosGetMemoryMap:
    pushad
    xor  bp, bp
    mov  eax, 0xe820
    mov  ebx, 0
    mov  ecx, 24
    mov  edx, 0x534d4150 ; SMAP
    int  0x15
    jc   .error

    cmp  eax, 0x534d4150
    jne  .error
    test ebx, ebx
    je   .error
    jmp  .start

.next_entry:
    mov  eax, 0xe820
    mov  ecx, 24,
    mov  edx, 0x534d4150
    int  0x15

.start:
    jcxz .skip_entry

.no_text:
    mov  ecx, [es:di + MemoryMapEntry.length]
    test ecx, ecx
    jne  .good_entry
    mov  ecx, [es:di + MemoryMapEntry.length + 4]
    jcxz .skip_entry

.good_entry:
    inc  bp
    add  di, 24

.skip_entry:
    cmp  ebx, 0
    jne  .next_entry
    jmp  .done

.error:
    stc
.done:
    popad
    ret

%endif ;__MEMORY_INC_INCLUDED__
