
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
    return ret;
}

void kpanic_internal(const char* msg, const char* file, int line)
{
    vga.SetColor(VGAColor::Red, VGAColor::Black);
    vga.Print("%? at %?:%?", msg, file, line);
    asm volatile("cli");
    asm volatile("hlt");
}

void zero_memory(void* ptr, u32 size)
{
    char* buf = (char*)ptr;
    for (u32 i = 0; i<size; ++i)
        buf[i] = 0;
}

u32 next_power_of_2(u32 size)
{
    u32 res;
    asm volatile("lzcnt %1, %0" : "=a"(res) : "dN"(size));
    res = 1 << res;
    if (res != size)
        res <<= 1;
    return res;
}

int atoi(const char* val, int valLen)
{
    bool posSign = true;
    while (*val == ' ') { ++val; --valLen; }
    if (*val == '-' || *val == '+')
    {
        posSign = *val == '+';
        ++val;
        --valLen;
    }

    int ival = 0;
    while (*val >= '0' && *val <= '9' && valLen > 0)
    {
        ival = ival * 10 + (*val - '0');
        ++val;
        --valLen;
    }
    return posSign ? ival : -ival;
}
