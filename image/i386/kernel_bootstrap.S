_start:
    call zero_bss
    call Kernel_entry
    ret

zero_bss:
    mov $__bss_start, %edi
    mov $__bss_end, %ecx
    sub %edi, %ecx
    shr $2, %ecx # size /= 4

    xor %eax, %eax
    cld
    rep stosl

    # remaining bytes
    mov $__bss_end, %ecx
    sub %edi, %ecx
    and $3, %ecx
    rep stosb

    ret

.globl _start
.extern __bss_start
.extern __bss_end
.extern Kernel_entry
