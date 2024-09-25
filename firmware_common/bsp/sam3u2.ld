LD_FEATURE("SANE_EXPR");

MEMORY
{
    ROM0 (rx!w) : ORIGIN = 0x00080000, LENGTH = 128K
    RAM0 (rwxa) : ORIGIN = 0x20000000, LENGTH = 16K
    RAM1 (rwxa) : ORIGIN = 0x20080000, LENGTH = 16K
    NAND_RAM (rwa) : ORIGIN = 0x20100000, LENGTH = 4224
}

ENTRY(_start);

SECTIONS
{
    _stack_size = 0x1000;
    _heap_size = 0x4000; /* All of RAM1 */

    .intvec (READONLY) : {
        *(.intvec)
    } >ROM0

    .text (READONLY) :
    {
        *(.text* .rodata*)
    } >ROM0

    .data : {
        _data_start = ABSOLUTE(.);
        *(.data*)
        _data_end = ABSOLUTE(.);
    } > RAM0 AT>ROM0
    _data_load = LOADADDR(.data);

    .bss (NOLOAD) : {
        _bss_start = ABSOLUTE(.);
        *(.bss*)
        *( COMMON*)
        _bss_end = ABSOLUTE(.);
    } >RAM0

    .stack (NOLOAD) : {
        . = ALIGN(8);
        _stack_limit = ABSOLUTE(.);
        . += _stack_size;
        _stack_init = ABSOLUTE(.);
    } > RAM0

    .heap (NOLOAD) : {
        . = ALIGN(8);
        _heap_start = ABSOLUTE(.);
        . += _heap_size;
        _heap_end = ABSOLUTE(.);
    } > RAM1
}