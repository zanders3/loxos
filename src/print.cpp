#include "print.h"
#include "vga.h"
#include "serial.h"

void puts(char c)
{
	vga_puts(c);
	serial_puts(c);
}

void printval(const char* fmt)
{
	for (; fmt && *fmt; ++fmt)
		puts(*fmt);
}

static void printval(unsigned int val, unsigned int radix)
{
	if (radix == 10 && (int)val < 0)
	{
		puts('-');
		val = -val;
	}
	else if (val == 0)
	{
		puts('0');
		return;
	}
	char buf[20];
	int i = 0;
	for (; val > 0; i++)
	{
		unsigned int digit = val % radix;
		buf[i] = (char)(digit < 10 ? '0' + digit : 'a' + digit - 10);
		val /= radix;
	}
	i--;
	for (;i>=0;i--)
		puts(buf[i]);
}

void printval(int val)
{
	printval(val, 10);
}

void printval(unsigned int val)
{
	printval("0x");
	printval(val, 16);
}
