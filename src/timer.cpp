#include "common.h"

#include "timer.h"

#include "isr.h"
#include "vga.h"

long ticks;

void timercallback(const Registers&)
{
	ticks++;
    vga.TickCursor();
}

void init_timer()
{
	ticks = 0;
	register_irq_handler(0, &timercallback);
	u32 divisor = 1193180 / 100;
	outb(0x43, 0x36);
	outb(0x40, (u8)(divisor & 0xFF));
	outb(0x40, (u8)((divisor >> 8) & 0xFF));
}
