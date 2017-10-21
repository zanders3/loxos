#include "print.h"
#include "idt.h"

extern "C" void kmain()
{
	printf("hello %? %? %?\n", 5, (unsigned int)0x3A, -29382);
	idt_init();
	asm("cli\nhlt");
}
