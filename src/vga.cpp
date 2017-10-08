#include "vga.h"
#include "common.h"

VGA vga;

#define WIDTH 80
#define HEIGHT 25

void VGA::Clear()
{
	cursorX = 0;
	cursorY = 0;
	SetColor(VGAColor::White, VGAColor::Black);
	
	short* vga = (short*)0xB8000;
	for (int i = 0; i<WIDTH*HEIGHT; i++)
		vga[i] = 0;
}

void VGA::SetColor(VGAColor fg, VGAColor bg)
{
	color = (int)bg << 12 | (int)fg << 8;
}

void VGA::Print(const char* str)
{
	for (; str && *str; ++str) 
		Puts(*str);
}

void VGA::Puts(char c)
{
	short* vga = (short*)0xB8000;
	if (c == '\n')
	{
		vga[cursorY*80 + cursorX] = color | ' ';
		cursorY++;
		cursorX = 0;
	}
	else if (c == '\r')
		cursorX = 0;
	else if (c >= ' ')
	{
		vga[cursorY*80 + cursorX] = color | c;
		cursorX++;
		if (cursorX >= WIDTH)
		{
			cursorX = 0;
			cursorY++;
		}
	}

	if (cursorY >= 25)
	{
		cursorY = 24;
		cursorX = 0;
		for (int i = 0; i<24*80; ++i)
			vga[i] = vga[i+80];
		for (int i = 24*80; i<25*80; ++i)
			vga[i] = 0;
	}
}

void VGA::Backspace()
{
	if (cursorX > 0)
	{
		short* vga = (short*)0xB8000;
		vga[cursorY*80 + cursorX] = color | ' ';
		--cursorX;
		vga[cursorY*80 + cursorX] = color | ' ';
	}
}

void VGA::TickCursor()//tick every 10ms
{
	cursorTimer++;
	if (cursorTimer >= 50)//every 500ms
	{
		short* vga = (short*)0xB8000;
		vga[cursorY*80 + cursorX] = color | (cursor ? '_' : ' ');
		cursor = !cursor;
		cursorTimer = 0;
	}
}

void VGA::PrintVal(const char* s)
{
	for (;*s;++s)
		Puts(*s);
}

void VGA::PrintVal(short value)
{
	PrintVal((int)value);
}

void VGA::PrintVal(int value)
{
	if (value < 0)
	{
		Puts('-');
		value = -value;
	}
	else if (value == 0)
	{
		Puts('0');
		return;
	}
	char buf[20];
	int i = 0;
	for (; value > 0; i++)
	{
		long digit = value % 10;
		buf[i] = '0' + digit;
		value /= 10;
	}
	i--;
	for (;i>=0;i--)
		Puts(buf[i]);
}

void VGA::PrintVal(unsigned short value)
{
	PrintVal((unsigned int)value);
}

void VGA::PrintVal(unsigned int value)
{
	if (value <= 0)
	{
		Puts('0');
		return;
	}

	char buf[12];
	int i = 0;
	for (; value > 0; i++)
	{
		unsigned int digit = value % 16;
		buf[i] = (char)(digit < 10 ? '0' + digit : 'a' + digit - 10);
		value /= 16;
	}
	i--;
	for (;i>=0;i--)
		Puts(buf[i]);
}

void VGA::PrintVal(char value)
{
	Puts(value);
}
