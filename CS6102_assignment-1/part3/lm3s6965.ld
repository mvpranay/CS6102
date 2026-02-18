/* Memory layout for LM3S6965 (QEMU default Cortex-M4) */
MEMORY
{
    FLASH (rx)  : ORIGIN = 0x00000000, LENGTH = 256K
    RAM (rwx)   : ORIGIN = 0x20000000, LENGTH = 64K
}

/* Stack size */
_estack = ORIGIN(RAM) + LENGTH(RAM);

SECTIONS
{
    .text :
    {
        KEEP(*(.isr_vector))
        *(.text)
        *(.text*)
        *(.rodata)
        *(.rodata*)
        . = ALIGN(4);
        _etext = .;
    } > FLASH

    .data : AT(_etext)
    {
        _sdata = .;
        *(.data)
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } > RAM

    _sidata = LOADADDR(.data);

    .bss :
    {
        _sbss = .;
        *(.bss)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } > RAM

    /* Heap section */
    .heap :
    {
        . = ALIGN(4);
        _heap_start = .;
        . = . + 0x4000;  /* 16KB heap */
        _heap_end = .;
    } > RAM
    
    /DISCARD/ :
    {
        *(.ARM.exidx*)
    }
}
