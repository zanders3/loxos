#include "print.h"

const int Width = 80;
const int Height = 25;
const int Color = 0x0F00;
int g_cursorY = 0, g_cursorX = 0;

void puts(char c)
{
	short* vga = (short*)0xb8000;
	if (c == '\n')
	{
		vga[g_cursorY*Width + g_cursorX] = 0;
		++g_cursorY;
		g_cursorX = 0;
	}
	else if (c == '\r')
		g_cursorX = 0;
	else if (c >= ' ')
	{
		vga[g_cursorY*Width + g_cursorX] = Color | c;
		++g_cursorX;
		if (g_cursorX >= Width)
		{
			g_cursorX = 0;
			++g_cursorY;
		}
	}
	else if (c == 0x10)//backspace
	{
		if (g_cursorX > 0)
		{
			vga[g_cursorY*Width + g_cursorX] = 0;
			--g_cursorX;
			vga[g_cursorY*Width + g_cursorX] = 0;
		}
	}

	if (g_cursorY >= Height)
	{
		g_cursorY = Height - 1;
		g_cursorX = 0;
		for (int i = 0; i<(Height-1)*Width; ++i)
			vga[i] = vga[i+80];
		for (int i = (Height-1)*Width; i<Width*Height; ++i)
			vga[i] = 0;
	}
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
