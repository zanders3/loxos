#include "gdt.h"

GDTEntry gdt_entries[5];
GDTPointer gdt_pointer;
IDTEntry idt_entries[256];
IDTPointer idt_pointer;

extern "C" void gdt_flush(u32);
extern "C" void idt_flush(u32);

static void gdt_setup(int idx, u32 base, u32 limit, u8 access, u8 granularity)
{
    GDTEntry& entry = gdt_entries[idx];
    entry.base_low = base & 0xFFFF;
    entry.base_middle = (base >> 16) & 0xFF;
    entry.base_high = (base >> 24) & 0xFF;
    entry.limit_low = limit & 0xFFFF;
    entry.granularity = (limit >> 16) & 0x0F;
    entry.granularity |= granularity & 0xF0;
    entry.access = access;
}

void init_gdt()
{
    gdt_pointer.limit = (sizeof(GDTEntry) * 5) - 1;
    gdt_pointer.base = (u32)&gdt_entries;

    // Access byte format:
    // P | DPL 2 | DT | Type 3
    // P = Segment present? (1 = true)
    // DPL = Privilege level - Kernel ring 0-3
    // DT = Descriptor type (1 = code, 0 = data)
    // Type

    // Granularity byte format:
    // G | D | 0 | A
    // G = Granularity (0 = 1 byte, 1 = 1kbyte)
    // D = Operand size (0 = 16bit, 1 = 32bit)
    // 0 = Always 0
    // A = System use always 0
    // => 0xCF sets G + D flags
    gdt_setup(0, 0, 0, 0, 0); // NULL segment
    gdt_setup(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);//Code segment (present, ring 0, DT = 1, Type = 2)
    gdt_setup(2, 0, 0xFFFFFFFF, 0x92, 0xCF);//Data segment (present, ring 0, DT = 0, Type = 2)
    gdt_setup(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);//User mode code segment (present, ring 3, DT = 1, Type = 2)
    gdt_setup(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);//User mode data segment (present, ring 3, DT = 0, Type = 2)
    gdt_flush((u32)&gdt_pointer);
}

static void idt_setup(int idx, u32 address, u16 sel, u8 flags)
{
    IDTEntry& entry = idt_entries[idx];
    entry.addr_lo = address & 0xFFFF;
    entry.addr_high = (address >> 16) & 0xFFFF;
    entry.sel = sel;
    entry.always0 = 0;
    entry.flags = flags;
}

extern "C" void isr0();
extern "C" void isr1();
extern "C" void isr2();
extern "C" void isr3();
extern "C" void isr4();
extern "C" void isr5();
extern "C" void isr6();
extern "C" void isr7();
extern "C" void isr8();
extern "C" void isr9();
extern "C" void isr10();
extern "C" void isr11();
extern "C" void isr12();
extern "C" void isr13();
extern "C" void isr14();
extern "C" void isr15();

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
    idt_setup(0, (u32)isr0, 0x08, 0x8E);
    idt_setup(1, (u32)isr1, 0x08, 0x8E);
    idt_setup(2, (u32)isr2, 0x08, 0x8E);
    idt_setup(3, (u32)isr3, 0x08, 0x8E);
    idt_setup(4, (u32)isr4, 0x08, 0x8E);
    idt_setup(5, (u32)isr5, 0x08, 0x8E);
    idt_setup(6, (u32)isr6, 0x08, 0x8E);
    idt_setup(7, (u32)isr7, 0x08, 0x8E);
    idt_setup(8, (u32)isr8, 0x08, 0x8E);
    idt_setup(9, (u32)isr9, 0x08, 0x8E);

    idt_flush((u32)&idt_pointer);
}
