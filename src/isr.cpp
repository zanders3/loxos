#include "isr.h"
#include "vga.h"

IDTEntry idt_entries[256];
IDTPointer idt_pointer;

static void idt_setup(int idx, u32 address, u16 sel, u8 flags)
{
    IDTEntry& entry = idt_entries[idx];
    entry.addr_lo = address & 0xFFFF;
    entry.addr_high = (address >> 16) & 0xFFFF;
    entry.sel = sel;
    entry.always0 = 0;
    entry.flags = flags;
}

extern "C" u32 isr_loc;

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
    for (int i = 0; i<32; i++)
    	idt_setup(i, isrLoc[i], 0x08, 0x8E);

    __asm__ __volatile__("lidt (%0)\n" :: "r"(&idt_pointer));

    //Remap IRQ table
	/*outb(0x20, 0x11);
	outb(0xA0, 0x11);
	outb(0x21, 0x20);
	outb(0xA1, 0x28);
	outb(0x21, 0x04);
	outb(0xA1, 0x02);
	outb(0x21, 0x01);
	outb(0xA1, 0x01);
	outb(0x21, 0x0);
	outb(0xA1, 0x0);

	asm volatile("sti");*/
}

extern "C" void isr_handler(const Registers regs)
{
	//Handle PIC interrupt
	/*if (regs.interrupt >= 0x28)
		outb(0xA0, 0x20);//send reset signal to slave PIC
	outb(0x20, 0x20);//send reset signal to master*/

	/*if (handlers[regs.interrupt])
		handlers[regs.interrupt](regs);*/
	vga.Print("Handled interrupt: %? %?\n", (int)regs.interrupt, (int)regs.err_code);
	if ((int)regs.interrupt == 13)
		kpanic("");
}
