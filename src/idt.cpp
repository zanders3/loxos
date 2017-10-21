
#include "print.h"

typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

struct Registers
{
	u32 ds;
	u32 edi, esi, ebp, esp, ebx, edx, ecx, eax;
	u32 interrupt, err_code;
	u32 eip, cs, eflags, useresp, ss;
} __attribute__((packed));

struct IDTEntry
{
    u16 addr_lo;//target address low 16 bits
    u16 sel;//memory offset to kernel segment selector
    u8 always0;//always 0
    u8 flags;//flags
    u16 addr_high;//target address high 16 bits
} __attribute__((packed));

struct IDTPointer
{
    u16 limit;
    u32 base;
} __attribute__((packed));

IDTEntry idt_entries[256] = {};
IDTPointer idt_pointer;

static const char* isrErrors[19] = {
	"Division by zero", 
	"Debug exception",
	"Non maskable interrupt",
	"Breakpoint exception",
	"'Into detected overflow'",
	"Out of bounds exception",
	"Invalid opcode exception",
	"No coprocessor exception",
	"Double fault",
	"Coprocessor segment overrun",
	"Bad TSS",
	"Segment not present",
	"Stack fault",
	"General protection fault",
	"Page fault",
	"Coprocessor fault",
	"Alignment check exception",
	"Machine check exception"
};

__attribute__ ((interrupt))
void isr_handler(const Registers& regs)
{
	printf("Unhandled interrupt:\n%? (%?) code %? at 0x%?\n",
		regs.interrupt <= 18 ? isrErrors[regs.interrupt] : "Reserved",
		(int)regs.interrupt, 
		(int)regs.err_code,
		regs.eip);

	// Interpret selector error code for Bad TSS to General Protection fault errors
	// http://wiki.osdev.org/Exceptions#Selector_Error_Code
	if (regs.interrupt >= 10 && regs.interrupt <= 13)
	{
		int tableType = (regs.err_code >> 1) & 0x3;
		printf("using %? %? at index %?\n",
			regs.err_code & 0x1 ? "External" : "Internal",
			tableType == 0 ? "GDT" :
			tableType == 1 ? "IDT" :
			tableType == 2 ? "LDT" :
			"IDT",
			(int)(regs.err_code >> 3)
		);
	}
	asm ("cli\nhlt");
}

void idt_init()
{
	idt_pointer.limit = (sizeof(IDTEntry) * 256) - 1;
	idt_pointer.base = (u32)&idt_entries;
}

