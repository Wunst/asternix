ENTRY(_start)

SECTIONS
{
    . = 1M;

    .text : ALIGN(4K)
    {
        KEEP(*(.multiboot))
        
        *(.text .text.*)
    }
    
    .data : ALIGN(4K)
    {
        *(.data .data.*)
    }

    .bss : ALIGN(4K)
    {
        *(.bss .bss.*)
    }

    .rodata : ALIGN(4K)
    {
        *(.rodata .rodata.*)
    }
}
