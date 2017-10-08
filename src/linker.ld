ENTRY(start)

SECTIONS {
	. = 1M;
	.text :
	{
        *(.multiboot_header)
		*(.text)
	}
    
    .rodata BLOCK(4K) : ALIGN(4K)
    {
    	*(.rodata)
    }

    .data BLOCK(4K) : ALIGN(4K)
    {
    	*(.data)
    }

    .bss BLOCK(4K) : ALIGN(4K)
    {
    	*(COMMON)
    	*(.bss)
    }
}