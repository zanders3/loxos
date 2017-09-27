#include "isr.h"
#include "vga.h"

IDTEntry idt_entries[256];
IDTPointer idt_pointer;

//interrupt handler assembly code locations
//defined in interrupts.asm
extern "C" u32 isr_loc;

static void idt_setup(int idx, u32 address, u16 sel, u8 flags)
{
    IDTEntry& entry = idt_entries[idx];
    entry.addr_lo = address & 0xFFFF;
    entry.addr_high = (address >> 16) & 0xFFFF;
    entry.sel = sel;
    entry.always0 = 0;
    entry.flags = flags;
}

void init_idt()
{
    idt_pointer.limit = (sizeof(IDTEntry) * 256) - 1;
    idt_pointer.base = (u32)&idt_entries;

    // 0x08 is offset to kernel code segment selector
    // Flags byte format:
    // P | DPL 2 | Misc 4
    // P = Interrupt present? (1 = true)
    // DPL = Privilege level - Kernel ring 0-3
    // Misc = Always 0xE
    // 0x8E sets present, ring 0
    zero_memory(&idt_entries, sizeof(IDTEntry)*256);
    u32* isrLoc = &isr_loc;
    for (int i = 0; i<48; i++)
    	idt_setup(i, isrLoc[i], 0x08, 0x8E);

    //Load IDT instruction
    __asm__ __volatile__("lidt (%0)\n" :: "r"(&idt_pointer));

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

	//Enable interrupts
	asm volatile("sti");
}

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

extern "C" void isr_handler(const Registers regs)
{
	vga.Print("Unhandled interrupt:\n%? (%?) code %? at 0x%?\n",
		regs.interrupt <= 18 ? isrErrors[regs.interrupt] : "Reserved",
		(int)regs.interrupt, 
		(int)regs.err_code,
		regs.eip);

	// Interpret selector error code for Bad TSS to General Protection fault errors
	// http://wiki.osdev.org/Exceptions#Selector_Error_Code
	if (regs.interrupt >= 10 && regs.interrupt <= 13)
	{
		int tableType = (regs.err_code >> 1) & 0x3;
		vga.Print("using %? %? at index %?\n",
			regs.err_code & 0x1 ? "External" : "Internal",
			tableType == 0 ? "GDT" :
			tableType == 1 ? "IDT" :
			tableType == 2 ? "LDT" :
			"IDT",
			(int)(regs.err_code >> 3)
		);
	}
	kpanic("");
}

extern "C" void irq_handler(const Registers regs)
{
	if (regs.interrupt >= 8)
		outb(0xA0, 0x20);
	outb(0x20, 0x20);

	vga.Print("IRQ %?\n", (int)regs.interrupt);
}
