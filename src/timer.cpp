#include "common.h"

#include "timer.h"

#include "interrupt_handler.h"

Timer timer;

void timercallback(const Registers& regs)
{
}

void Timer::Init(u32 freq)
{
	interrupts.RegisterHandler(32, &timercallback);
	u32 divisor = 1193180 / freq;
	outb(0x43, 0x36);
	outb(0x40, (u8)(divisor & 0xFF));
	outb(0x40, (u8)((divisor >> 8) & 0xFF));
}
