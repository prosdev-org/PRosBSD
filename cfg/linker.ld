ENTRY(_start)

PHDRS
{
  text PT_LOAD FLAGS(5);    /* R-X */
  rodata PT_LOAD FLAGS(4);  /* R-- */
  data PT_LOAD FLAGS(6);    /* RW- */
  bss PT_LOAD FLAGS(6);     /* RW- */
}

SECTIONS
{
  . = 0x100000;

  .text : AT(ADDR(.text)) {
    *(.text*)
  } :text

  .rodata : AT(ADDR(.rodata)) {
    *(.rodata*)
  } :rodata

  .data : AT(ADDR(.data)) {
    __data_start = .;
    *(.data*)
    __data_end = .;
  } :data

  .bss : AT(ADDR(.bss)) {
    __bss_start = .;
    *(.bss*)
    *(COMMON)
    __bss_end = .;
  } :bss

  __kernel_end = .;
}
