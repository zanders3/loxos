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
