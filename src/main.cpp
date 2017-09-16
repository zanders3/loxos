#include "common.h"
#include "vga.h"
#include "interrupt_handler.h"
#include "timer.h"

extern "C" int kmain()
{
	vga.Clear();
	vga.Print("loxos\nHello world");

	interrupts.SetupPIC();
	asm volatile("sti");
	timer.Init(50);

	vga.Print("back\n");
	while (true) {}
	return 0;
}
