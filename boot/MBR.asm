[BITS 16]
[ORG 0x7C00]

FAT12_BYTES_PER_SECTOR equ 512
FAT12_NUMBER_OF_FATS equ 2
FAT12_SECTORS_PER_FAT equ 9
FAT12_FAT_TABLE_SIZE equ FAT12_SECTORS_PER_FAT * FAT12_BYTES_PER_SECTOR
FAT12_FAT_TABLE_SECTOR equ 1
FAT12_DIRECTORY_TABLE_SIZE equ 7168
FAT12_DIRECTORY_TABLE_SECTOR equ FAT12_FAT_TABLE_SECTOR + FAT12_NUMBER_OF_FATS*FAT12_SECTORS_PER_FAT
FAT12_DIRECTORY_TABLE_SECTOR_COUNT equ FAT12_DIRECTORY_TABLE_SIZE / FAT12_BYTES_PER_SECTOR
FAT12_DATA_SECTOR equ FAT12_DIRECTORY_TABLE_SECTOR + FAT12_DIRECTORY_TABLE_SECTOR_COUNT

; 0600-0E00 SETUP.BIN
; 7E00-???? KERNEL.BIN

SETUPSEG           equ 0x0060
SETUPMAX        equ 2048/16
KERNSEG          equ 0x07E0
KERNEL_SEGMENT_DIFF equ 0x0200
FAT_TABLE_OFFSET equ (KERNSEG + KERNEL_SEGMENT_DIFF)*16
FAT_DIRECTORY_TABLE_OFFSET equ FAT_TABLE_OFFSET + FAT12_FAT_TABLE_SIZE

jmp boot

times 3-($-$$) db 0
bpbOEM db "    PRos"
bpbBytesPerSector DW 512
bpbSectorsPerCluster DB 1
bpbReservedSectors DW 1
bpbNumberOfFATs DB 2
bpbRootEntries DW 224
bpbTotalSectors DW 2880
bpbMedia DB 0xf0
bpbSectorsPerFAT DW 9
bpbSectorsPerTrack DW 18
bpbHeadsPerCylinder DW 2
bpbHiddenSectors DD 0
bpbTotalSectorsBig DD 0
bsDriveNumber DB 0
bsUnused DB 0
bsExtBootSignature DB 0x29
bsSerialNumber DD 0xa0a1a2a3
bsVolumeLabel DB "FLOPPY "
bsFileSystem DB "FAT12   "

boot:
        cld
        cli
        xor bp, bp
        mov ss, bp
        mov sp, SETUPSEG*16
        sti
        mov [drive], dl

read_fat_table:
        xor ax, ax
        mov es, ax
        mov bx, FAT_TABLE_OFFSET
        mov al, FAT12_SECTORS_PER_FAT
        mov dx, FAT12_FAT_TABLE_SECTOR
        call read_sector
read_directory_table:
        xor ax, ax
        mov es, ax
        mov bx, FAT_DIRECTORY_TABLE_OFFSET
        mov al, FAT12_DIRECTORY_TABLE_SECTOR_COUNT
        mov dx, FAT12_DIRECTORY_TABLE_SECTOR
        call read_sector

find_setup_bin:
        xor ax, ax
        mov es, ax
        mov di, setup_bin_filename
        call find_file_entry
read_setup_bin:
        mov dx, FAT12_DATA_SECTOR-2
        add dx, [bx+26]
        mov ax, SETUPSEG
        mov es, ax
        mov ax, [bx+28]
        mov cl, 9
        shr ax, cl
        inc ax
        xor bx, bx
        call read_sector

find_kernel_bin:
        xor ax, ax
        mov es, ax
        mov di, kernel_bin_filename
        call find_file_entry
read_kernel_bin:
        mov dx, FAT12_DATA_SECTOR-2
        add dx, [bx+26]
        mov ax, KERNSEG
        mov es, ax
        mov ax, [bx+28]
        mov cl, 9
        shr ax, cl
        inc ax
        xor bx, bx
        call read_sector

        jmp SETUPSEG:0

halt:
        cli
        hlt

read_sector:
        push ax
        mov ax, dx
        mov ch, 18
        div ch
        mov cl, ah
        inc cl ; sector
        xor ah, ah
        mov ch, 2
        div ch
        mov ch, al ; cylinder
        mov dh, ah ; head
        pop ax
        mov dl, [drive]
        mov ah, 2 ; read sectors from drive
        int 13h
        jc read_error
        ret

find_file_entry:
        xor ax, ax
        mov ds, ax
        mov dx, di
        mov es, ax
        mov bx, FAT_DIRECTORY_TABLE_OFFSET
        mov si, bx
find_next_file_entry:
        cmp byte [si], 0
        jz file_entry_not_found
        mov di, dx
        mov cx, 8+3
        rep cmpsb
        je file_entry_found
        add bx, 32
        mov si, bx
        jmp find_next_file_entry
file_entry_not_found:
        mov si, not_found
        jmp print_error_message
file_entry_found:
        ret

read_error:
        mov al, ah
        call print_number
        xor ax, ax
        mov ds, ax
        mov si, read_error_message

print_error_message:
        lodsb
        cmp al,0
        je halt
        mov ah, 0x0E
        mov bh, 0x00
        int 10h
        jmp print_error_message

print_number:
        push ax
        shr al, 4
        call print_digit
        pop ax
        and al, 15
        call print_digit
        ret

print_digit:
        cmp al, 9
        ja tens
        add al, '0'
        jmp ones
        tens:
        add al, 'A'-10
        ones:
        mov ah, 0x0E
        mov bh, 0x00
        int 10h
        ret

not_found           db "Entry not found!", 0
read_error_message  db "Read error!", 0
setup_bin_filename     db "SETUP   BIN"
kernel_bin_filename  db "KERNEL  BIN"

drive db 0

times 510-($-$$) db 0
dw 0xAA55