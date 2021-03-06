%ifndef __PAGING_INC_INCLUDED__
%define __PAGING_INC_INCLUDED__

bits 32
section .text

; page directory table
%define		PAGE_DIR			0x9C000

; 0th page table. Address must be 4KB aligned
%define		PAGE_TABLE_0		0x9D000

; 768th page table. Address must be 4KB aligned
%define		PAGE_TABLE_768		0x9E000

; each page table has 1024 entries
%define		PAGE_TABLE_ENTRIES	1024

; attributes (page is present;page is writable; supervisor mode)
%define		PAGE_DEFAULT_ATTRS  3

EnablePaging:
    pusha

    ; identity map first table
    mov eax, PAGE_TABLE_0
    mov ebx, 0 | PAGE_DEFAULT_ATTRS
    mov ecx, PAGE_TABLE_ENTRIES
    .loop:
        mov dword [eax], ebx
        add eax, 4
        add ebx, 4096
    loop .loop

    ; map 3gb to 1mb
    mov eax, PAGE_TABLE_768
    mov ebx, 0x100000 | PAGE_DEFAULT_ATTRS
    mov ecx, PAGE_TABLE_ENTRIES
    .loop2:
        mov dword [eax], ebx
        add eax, 4
        add ebx, 4096
    loop .loop2

    ; set up entries in dirtable
    mov eax, PAGE_TABLE_0   | PAGE_DEFAULT_ATTRS
    mov dword [PAGE_DIR], eax
    mov eax, PAGE_TABLE_768 | PAGE_DEFAULT_ATTRS
    mov dword [PAGE_DIR + (768*4)], eax

    ; install dirtable
    mov eax, PAGE_DIR
    mov cr3, eax

    ; enable paging
    mov eax, cr0
    or  eax, 0x80000000
    mov cr0, eax

    popa
    ret

%endif
