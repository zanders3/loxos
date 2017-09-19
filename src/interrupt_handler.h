#pragma once

#include "common.h"

struct Registers
{
	u64 sp, ax, bx, cx, dx, di, si, r8, r9, r10, r11, r12, r13, r14, r15, bp;
	u64 interrupt, ip, cs, flags;
};

typedef void (*InterruptHandler)(const Registers&);

class Interrupts
{
public:
	//Initializes the programmable interrupt controllers
	void Setup();
	//Registers a callback from a PIC
	inline void RegisterHandler(u8 n, InterruptHandler handler) { handlers[n] = handler; }
	//Called from kfault_handler() in interrupt_handler.cpp
	//kfault_handler() is called from interrupts.asm and the interrupt table is initd in boot.asm
	void CallHandler(const Registers& regs);
private:
	InterruptHandler handlers[256];
};
extern Interrupts interrupts;
