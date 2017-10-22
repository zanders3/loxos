#include "idt.h"
#include "print.h"

struct Registers
{
	u32 ds;
	u32 edi, esi, ebp, esp, ebx, edx, ecx, eax;
} __attribute__((packed));

struct IDTEntry
{
    u16 addr_lo;    //target address low 16 bits
    u16 sel;        //memory offset to kernel segment selector
    u8 always0;     //always 0
    u8 flags;       //flags
    u16 addr_high;  //target address high 16 bits
} __attribute__((packed));

struct IDTPointer
{
    u16 limit;
    u32 base;
} __attribute__((packed));

IDTEntry g_idt_entries[256] = {};
IDTPointer g_idt_pointer;

static const char* isrErrors[19] = {
	"Division by zero", 
	"Debug exception",
	"Non maskable interrupt",
	"Breakpoint exception",
	"Into detected overflow",
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

void isr_error_handler(int int_no, int error_code)
{
	asm("cli");
	printf("Unhandled interrupt:\n%? (%?) code %?\n",
		int_no <= 18 ? isrErrors[int_no] : "Reserved", int_no, error_code);

	// Interpret selector error code for Bad TSS to General Protection fault errors
	// http://wiki.osdev.org/Exceptions#Selector_Error_Code
	if (int_no >= 10 && int_no <= 13)
	{
		int tableType = (error_code >> 1) & 0x3;
		printf("using %? %? at index %?\n",
			error_code & 0x1 ? "External" : "Internal",
			tableType == 0 ? "GDT" :
			tableType == 1 ? "IDT" :
			tableType == 2 ? "LDT" :
			"IDT",
			(int)(error_code >> 3)
		);
	}
	asm ("cli\nhlt");
}

#define INT(i)    __attribute__((interrupt)) void isr##i(int*) { isr_error_handler(i, 0); }
#define INTERR(i) __attribute__((interrupt)) void isr##i(int*, int err_code) { isr_error_handler(i, err_code); }
#include "isr_defs.h"
#undef INT
#undef INTERR

void idt_setup(int i, u32 address)
{
	IDTEntry& entry = g_idt_entries[i];
	entry.addr_lo = address & 0xFFFF;
	entry.sel = 0x8;
	entry.flags = 0x8E;
	entry.addr_high = (address >> 16) & 0xFFFF;
}

void outb(u16 port, u8 value)
{
	asm volatile ("outb %1, %0" : : "dN" (port), "a" (value));
}

u8 inb(u16 port)
{
	u8 ret;
	asm volatile("inb %1,%0" : "=a" (ret) : "dN" (port));
	return ret;
}

void idt_init()
{
	g_idt_pointer.limit = (sizeof(IDTEntry) * 256) - 1;
	g_idt_pointer.base = (u32)&g_idt_entries;

	#define INT(i) idt_setup(i, (u32)&isr##i);
	#define INTERR(i) idt_setup(i, (u32)&isr##i);
	#include "isr_defs.h"
	#undef INT
	#undef INTERR

	asm volatile("lidt (%0)" :: "r"(&g_idt_pointer));

	//Remap IRQ table from IRQ 0-15 to INT 32-47
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
}

void idt_irq_ack(int irq)
{
	if (irq > 7)
		outb(0xA0, 0x20);
	outb(0x20, 0x20);
}

