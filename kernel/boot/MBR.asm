.code16
.globl _start

.set FAT12_BYTES_PER_SECTOR, 512
.set FAT12_NUMBER_OF_FATS, 2
.set FAT12_SECTORS_PER_FAT, 9
.set FAT12_FAT_TABLE_SIZE, FAT12_SECTORS_PER_FAT * FAT12_BYTES_PER_SECTOR
.set FAT12_FAT_TABLE_SECTOR, 1
.set FAT12_DIRECTORY_TABLE_SIZE, 7168
.set FAT12_DIRECTORY_TABLE_SECTOR, FAT12_FAT_TABLE_SECTOR + FAT12_NUMBER_OF_FATS * FAT12_SECTORS_PER_FAT
.set FAT12_DIRECTORY_TABLE_SECTOR_COUNT, FAT12_DIRECTORY_TABLE_SIZE / FAT12_BYTES_PER_SECTOR
.set FAT12_DATA_SECTOR, FAT12_DIRECTORY_TABLE_SECTOR + FAT12_DIRECTORY_TABLE_SECTOR_COUNT

// 0600-0E00 SETUP.BIN
// 7E00-???? KERNEL.BIN

.set SETUPSEG, 0x0060
.set KERNSEG, 0x07E0
.set KERNEL_SEGMENT_DIFF, 0x0200
.set FAT_TABLE_OFFSET, (KERNSEG + KERNEL_SEGMENT_DIFF)*16
.set FAT_DIRECTORY_TABLE_OFFSET, FAT_TABLE_OFFSET + FAT12_FAT_TABLE_SIZE

_start:
    jmp boot

.fill 3 - (. - _start), 1, 0

bpbOEM: .ascii " PRosBSD"
bpbBytesPerSector: .word 512
bpbSectorsPerCluster: .byte 1
bpbReservedSectors: .word 1
bpbNumberOfFATs: .byte 2
bpbRootEntries: .word 224
bpbTotalSectors: .word 2880
bpbMedia: .byte 0xf0
bpbSectorsPerFAT: .word 9
bpbSectorsPerTrack: .word 18
bpbHeadsPerCylinder: .word 2
bpbHiddenSectors: .long 0
bpbTotalSectorsBig: .long 0
bsDriveNumber: .byte 0
bsUnused: .byte 0
bsExtBootSignature: .byte 0x29
bsSerialNumber: .long 0xa0a1a2a3
bsVolumeLabel: .ascii "FLOPPY     "
bsFileSystem: .ascii "FAT12   "

boot:
    cld
    cli
    xorw %bp, %bp
    movw %bp, %ss
    movw $(SETUPSEG*16), %sp
    sti
    movb %dl, drive

read_fat_table:
    xorw %ax, %ax
    movw %ax, %es
    movw $FAT_TABLE_OFFSET, %bx
    movb $FAT12_SECTORS_PER_FAT, %al
    movw $FAT12_FAT_TABLE_SECTOR, %dx
    call read_sector

read_directory_table:
    xorw %ax, %ax
    movw %ax, %es
    movw $FAT_DIRECTORY_TABLE_OFFSET, %bx
    movb $FAT12_DIRECTORY_TABLE_SECTOR_COUNT, %al
    movw $FAT12_DIRECTORY_TABLE_SECTOR, %dx
    call read_sector

find_setup_bin:
    xorw %ax, %ax
    movw %ax, %es
    movw $setup_bin_filename, %di
    call find_file_entry

read_setup_bin:
    movw $FAT12_DATA_SECTOR-2, %dx
    addw 26(%bx), %dx
    movw $SETUPSEG, %ax
    movw %ax, %es
    movw 28(%bx), %ax
    movb $9, %cl
    shrw %cl, %ax
    incw %ax
    xorw %bx, %bx
    call read_sector

find_kernel_bin:
    xorw %ax, %ax
    movw %ax, %es
    movw $kernel_bin_filename, %di
    call find_file_entry

read_kernel_bin:
    movw $FAT12_DATA_SECTOR-2, %dx
    addw 26(%bx), %dx
    movw $KERNSEG, %ax
    movw %ax, %es
    movw 28(%bx), %ax
    movb $9, %cl
    shrw %cl, %ax
    incw %ax
    xorw %bx, %bx
    call read_sector
    ljmp $SETUPSEG, $0

halt:
    cli
    hlt

read_sector:
    pushw %ax
    movw %dx, %ax
    movb $18, %ch
    divb %ch
    movb %ah, %cl
    incb %cl # sector
    xorb %ah, %ah
    movb $2, %ch
    divb %ch
    movb %al, %ch # cylinder
    movb %ah, %dh # head
    popw %ax
    movb drive, %dl
    movb $2, %ah # read sectors from drive
    int $0x13
    jc read_error
    ret

find_file_entry:
    xorw %ax, %ax
    movw %ax, %ds
    movw %di, %dx
    movw %ax, %es
    movw $FAT_DIRECTORY_TABLE_OFFSET, %bx
    movw %bx, %si

find_next_file_entry:
    cmpb $0, (%si)
    jz file_entry_not_found
    movw %dx, %di
    movw $11, %cx
    rep cmpsb
    je file_entry_found
    addw $32, %bx
    movw %bx, %si
    jmp find_next_file_entry

file_entry_not_found:
    movw $not_found, %si
    jmp print_error_message

file_entry_found:
    ret

read_error:
    movb %ah, %al
    call print_number
    xorw %ax, %ax
    movw %ax, %ds
    movw $read_error_message, %si

print_error_message:
    lodsb
    cmpb $0, %al
    je halt
    movb $0x0E, %ah
    movb $0x00, %bh
    int $0x10
    jmp print_error_message

print_number:
    pushw %ax
    shrb $4, %al
    call print_digit
    popw %ax
    andb $15, %al
    call print_digit
    ret

print_digit:
    cmpb $9, %al
    ja 1f
    addb $'0', %al
    jmp 2f
1:
    addb $'A'-10, %al
2:
    movb $0x0E, %ah
    movb $0x00, %bh
    int $0x10
    ret

not_found: .asciz "Entry not found!"
read_error_message: .asciz "Read error!"
setup_bin_filename: .ascii "SETUP   BIN"
kernel_bin_filename: .ascii "KERNEL  BIN"
drive: .byte 0

.fill 510 - (. - _start), 1, 0
.word 0xAA55
