#include "print.h"
#include "idt.h"
#include "keyboard.h"
#include "serial.h"
#include "display.h"

long ticks = 0;

__attribute__((interrupt)) void timer(int*)
{
	idt_irq_ack(0);
	++ticks;
}

static void timer_init()
{
	const u32 divisor = 1193180;
	outb(0x43, 0x36);
	outb(0x40, (u8)(divisor & 0xFF)); 
	outb(0x40, (u8)((divisor >> 8) & 0xFF));
	idt_irq_setup(0, (u32)&timer);
}

extern "C" void kmain()
{
	serial_init();
	display_init();

	idt_init();
	timer_init();
	keyboard_init();

	printf("Win!\n");
	asm ("cli\nhlt");
}
