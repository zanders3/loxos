ENTRY(start)

SECTIONS {
	. = 1M;

	.boot :
	{
		*(.multiboot_header)
	}

	_text_start = 0x104000;
	.text _text_start :
	{
		KEEP(*(.interrupts))
		*(.text)
	}
}