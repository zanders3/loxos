ENTRY(start)

SECTIONS {
	. = 1M;

	.boot :
	{
		*(.multiboot_header)
	}

	_text_start = 0x101000;
	.text _text_start :
	{
		KEEP(*(.interrupts))
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
    code_end = .;
}