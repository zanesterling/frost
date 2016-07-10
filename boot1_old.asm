;*********************************************
;    Boot1.asm
;    - A Simple Bootloader
;
;    Operating Systems Development Tutorial
;*********************************************

bits    16                        ; We are still in 16 bit Real Mode

org    0x7c00                        ; We are loaded by BIOS at 0x7C00

start:      jmp loader                    ; jump over OEM block

;*************************************************;
;    OEM Parameter block
;*************************************************;

TIMES 0Bh-$+start DB 0

bpbBytesPerSector:      DW 512
bpbSectorsPerCluster:     DB 1
bpbReservedSectors:     DW 1
bpbNumberOfFATs:     DB 2
bpbRootEntries:     DW 224
bpbTotalSectors:     DW 2880
bpbMedia:             DB 0xF0
bpbSectorsPerFAT:     DW 9
bpbSectorsPerTrack:     DW 18
bpbHeadsPerCylinder:     DW 2
bpbHiddenSectors:     DD 0
bpbTotalSectorsBig:     DD 0
bsDriveNumber:         DB 0
bsUnused:             DB 0
bsExtBootSignature:     DB 0x29
bsSerialNumber:        DD 0xa0a1a2a3
bsVolumeLabel:         DB "MOS FLOPPY "
bsFileSystem:         DB "FAT12   "

msg:    DB "Welcome to my Operating System!", 0x0d, 0x0a, 0
imageName: DB "KRNLDR  SYS"
failureMessage: DB "Couldn't find second stage bootloader :(", 0x0d, 0x0a, 0

absoluteSector:    DB 0
absoluteHead:    DB 0
absoluteCylinder:    DB 0
cluster:        DW 0
bootLoaderStart:    DW 0
bootLoaderCurrentLoc: DW 0

;*************************************************;
;    Prints a string
;    DS:SI: null terminated string
;*************************************************;

Print:
    lodsb
    or    al, al
    jz    PrintDone
    mov    ah, 0eh
    int    10h
    jmp    Print
PrintDone:
    ret

;*************************************************;
;    Prints a specified number of characters
;    DS:SI: string
;    CX: number of chars to print
;*************************************************;

Printn:
    or cx, cx
    jz PrintnDone
    sub cx, 1

    lodsb
    mov ah, 0eh
    int 10h
    jmp Printn
PrintnDone:
    mov al, 0x0d
    mov ah, 0eh
    int 10h
    mov al, 0x0a
    mov ah, 0eh
    int 10h
    ret

;*************************************************;
;    DS:SI: string 1
;    DS:DI: string 2
;    CX: length of strings
;    AX=> 0 if strings are equal, 1 otherwise
;*************************************************;
Strncmp:
    cmp cx, 0
    je StrncmpDone

    lodsb
    mov bl, al
    xchg si, di
    lodsb
    cmp al, bl
    jne StrncmpFail
    dec cx
    inc si
    inc di
    jmp Strncmp
StrncmpFail:
    mov ax, 1
    ret
StrncmpDone:
    xor ax, ax
    ret


;*************************************************;
;    Converts a sector number to CHS
;    AX: sector
;*************************************************;

LBACHS:
    div WORD [bpbSectorsPerTrack]
    inc ah
    mov BYTE [absoluteSector], ah
    xor ah, ah

    div WORD [bpbHeadsPerCylinder]
    mov BYTE [absoluteHead], ah
    mov BYTE [absoluteCylinder], al
    ret

;*************************************************;
;    Reads sectors into memory
;    AX: start sector
;    BX: dest
;    CL: number of sectors to read
;    DX=>trash
;*************************************************;

ReadSectors:
    call LBACHS

    mov al, cl
    mov ch, BYTE [absoluteCylinder]
    mov cl, BYTE [absoluteSector]

    mov dh, BYTE [absoluteHead]
    mov dl, BYTE [bsDriveNumber]
    mov ah, 2h
    int 13h
    ret

;*************************************************;
;    Bootloader Entry Point
;*************************************************;

loader:

    ; print msg
    xor    ax, ax
    mov    ds, ax
    mov    es, ax
    mov    si, msg
    call    Print

    ; Load root dir into memory.
    ; get root dir length in sectors
    mov    ax, 0x0020 ; 32 bytes per entry
    mul    WORD [bpbRootEntries]
    div    WORD [bpbBytesPerSector]
    mov    cx, ax

    ; get start of root dir
    mov    al, [bpbNumberOfFATs]
    mul    BYTE [bpbSectorsPerFAT]
    add    ax, [bpbReservedSectors]

    mov    bx, 0x0200
    call   ReadSectors


    ; Find stage 2 file entry in root dir.
    mov    cx, WORD [bpbRootEntries]
    mov    di, 0x0200
.LOOP:
    push cx
    mov cx, 11
    mov si, di
    call Printn
    mov cx, 11
    mov si, imageName
    call Strncmp
    pop cx
    cmp ax, 0
    je LOAD_FAT
    dec cx
    cmp cx, 0
    je FAILURE
    add di, 32
    jmp .LOOP

;    ; Load FAT into memory.
LOAD_FAT:
    mov    ax, WORD [di + 0x1d] ; file size in sectors
    shr    ax, 1

    mov    ax, WORD [di + 0x1a] ; first cluster
    mov    [cluster], ax

    xor    ax, ax
    mov    al, BYTE [bpbNumberOfFATs]
    mul    WORD [bpbSectorsPerFAT]
    mov    cx, ax

    mov    dx, 0x0200
    add    dx, cx
    mov    WORD [bootLoaderStart], dx
    mov    WORD [bootLoaderCurrentLoc], dx

    mov    ax, 1
    mov    bx, 0x0200
    call   ReadSectors


;    ; Load clusters for file.
;LOAD_IMAGE:
;    mov    ax, WORD [cluster]
;    sub    ax, 2
;    mul    BYTE [bpbSectorsPerCluster]
;    mov    bx, ax
;    xor    ax, ax
;    mov    al, BYTE [bpbNumberOfFATs]
;    mul    BYTE [bpbSectorsPerFAT]
;    add    ax, 1
;    add    ax, bx
;
;    mov    bx, WORD [bootLoaderCurrentLoc]
;    xor    cx, cx
;    mov    cl, BYTE [bpbSectorsPerCluster]
;    call   ReadSectors
;
;    ; increment the current reading destination
;    xor    ax, ax
;    mov    al, BYTE [bpbSectorsPerCluster]
;    mul    BYTE [bpbBytesPerSector]
;    add    WORD [bootLoaderCurrentLoc], ax
;
;    ; get index into fat
;    mov    ax, WORD [cluster]
;    mov    bx, ax
;    shr    bx, 1
;    add    bx, ax
;
;    mov    dx, WORD [bx + 0x0200]
;    test   ax, 1
;    jnz    .ODD_CLUSTER
;
;.EVEN_CLUSTER:
;    and    dx, 0x0fff
;    jmp    .DONE
;
;.ODD_CLUSTER:
;    shr    dx, 4
;
;    jmp    WORD [bootLoaderStart]
;
;.DONE:
;    mov    WORD [cluster], dx
;    cmp    dx, 0xff0
;    jb     LOAD_IMAGE

FAILURE:
    xor    ax, ax
    mov    ds, ax
    mov    es, ax
    mov    si, failureMessage
    call   Print
HALT:
    cli
    hlt ; halt the system

times 510 - ($-$$) db 0    ; We have to be 512 bytes. Clear the rest of the bytes with 0

dw 0xAA55 ; Boot Signiture
