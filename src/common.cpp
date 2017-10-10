
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

void print_stacktrace(u32 maxFrames)
{
    u32* ebp = &maxFrames - 2;
    for (u32 i = 0; i<maxFrames; ++i)
    {
        u32 eip = ebp[1];
        if (eip == 0)
            break;
        ebp = (u32*)ebp[0];
        vga.Print("   0x%?", eip);
    }
    vga.Puts('\n');
}

void kpanic_internal(const char* msg, const char* file, int line)
{
    vga.SetColor(VGAColor::Red, VGAColor::Black);
    vga.Print("%? at %?:%?", msg, file, line);
    print_stacktrace(10);
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

void itoa(int value, char* buffer, int radix)
{
    if (value < 0)
    {
        buffer[0] = '-';
        buffer[1] = '\0';
        value = -value;
    }
    else if (value == 0)
    {
        buffer[0] = '+';
        buffer[1] = '\0';
        return;
    }
    char buf[20];
    int i = 0;
    for (; value > 0; i++)
    {
        long digit = value % radix;
        buf[i] = '0' + digit;
        value /= radix;
    }
    i--;
    int j = 0;
    for (;i>=0;i--)
        buffer[j++] = buf[i];
    buffer[j] = '\0';
}

int strlen(const char* p)
{
    int count = 0;
    while (*p != '\0')
    {
        ++count; ++p;
    }
    return count;
}

extern "C" void __cxa_pure_virtual()
{
    kpanic("called abstract virtual function");
}

void* operator new(size_t, void *p)
{
    return p;
}

void operator delete(void*)
{ 
    kpanic("called unsupported delete"); 
}
