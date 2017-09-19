#include "interrupt_handler.h"
#include "vga.h"

Interrupts interrupts;

void Interrupts::Setup()
{
	//Remap IRQ table
	outb(0x20, 0x11);
	outb(0xA0, 0x11);
	outb(0x21, 0x20);
	outb(0xA1, 0x28);
	outb(0x21, 0x04);
	outb(0xA1, 0x02);
	outb(0x21, 0x01);
	outb(0xA1, 0x01);
	outb(0x21, 0x0);
	outb(0xA1, 0x0);

	asm volatile("sti");
}

void Interrupts::CallHandler(const Registers& regs)
{ 
	if (regs.interrupt >= 0x28)
		outb(0xA0, 0x20);//send reset signal to slave PIC
	outb(0x20, 0x20);//send reset signal to master

	if (handlers[regs.interrupt])
		handlers[regs.interrupt](regs);
}

extern "C" void kfault_handler(const Registers& regs)
{
	//Handle PIC interrupt
	if (regs.interrupt >= 0x20 && regs.interrupt < 0x30)
		interrupts.CallHandler(regs);
	else
	{
		vga.SetColor(VGAColor::Red, VGAColor::Black);
		vga.Print("Unhandled Interrupt: %?\n", (int)regs.interrupt);
		while (true) {}
	}
}
