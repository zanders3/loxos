#pragma once

#include "common.h"

struct Registers
{
	u32 ds;
	u32 edi, esi, ebp, esp, ebx, edx, ecx, eax;
	u32 interrupt, err_code;
	u32 eip, cs, eflags, useresp, ss;
} __attribute__((__packed__));

typedef void (*InterruptHandler)(const Registers&);

struct IDTEntry
{
    u16 addr_lo;//target address low 16 bits
    u16 sel;//memory offset to kernel segment selector
    u8 always0;//always 0
    u8 flags;//flags (see init_idt)
    u16 addr_high;//target address high 16 bits
} __attribute__((packed));

struct IDTPointer
{
    u16 limit;
    u32 base;
} __attribute__((packed));

void init_idt();
void register_int_handler(int int_code, InterruptHandler handler);
void register_irq_handler(int irq_code, InterruptHandler handler);
