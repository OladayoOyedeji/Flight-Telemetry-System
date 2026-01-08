MEMORY
{
    FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 256K
    SRAM (rwx) : ORIGIN = 0x20000000, LENGTH = 64K
}

SECTIONS
{
    .text : { *(.text*) } > FLASH
    .data : { *(.data*) } > SRAM AT > FLASH
    .bss  : { *(.bss*)  } > SRAM
}
