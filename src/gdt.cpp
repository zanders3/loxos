#include "gdt.h"

GDTEntry gdt_entries[5];
GDTPointer gdt_pointer;


extern "C" void gdt_flush(u32);

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

