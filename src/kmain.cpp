#include "print.h"
#include "idt.h"
#include "keyboard.h"

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
	idt_init();
	timer_init();
	keyboard_init();
	asm volatile("sti");

	printf("LoxOS\n");
	while (true)
	{
		printf("> ");
		const char* input = keyboard_readline();
		printf("Got %?", input);
		asm volatile("hlt");
	}
}
