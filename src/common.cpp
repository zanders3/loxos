
#include "common.h"
#include "vga.h"

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

u16 inw(u16 port)
{
	u16 ret;
	asm volatile ("inw %1,%0" : "=a" (ret) : "dN" (port));
}

void kpanic(const char* msg)
{
    vga.SetColor(VGAColor::Red, VGAColor::Black);
    vga.Print(msg);
    asm volatile("cli");
    asm volatile("hlt");
}

void zero_memory(void* ptr, u32 size)
{
    char* buf = (char*)ptr;
    for (u32 i = 0; i<size; ++i)
        buf[i] = 0;
}

extern "C" u32 code_end;
u32 kallocator_loc;

void init_allocator()
{
    kallocator_loc = (u32)&code_end;
}

void align_allocator()
{
    kallocator_loc = (kallocator_loc & 0xFFFFF000) + 0x1000;
}

void* operator new[](u32 size)
{
    u32 loc = kallocator_loc;
    kallocator_loc += size;
    return (void*)loc;
}

void* operator new(u32 size)
{
    u32 loc = kallocator_loc;
    kallocator_loc += size;
    return (void*)loc;
}
