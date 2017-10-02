#pragma once

enum class VGAColor
{
	Black = 0, Red = 4, LightGray = 7, DarkGray = 8, White = 15
};

//Prints to the screen including string formatting
class VGA
{
public:
	void Clear();
	void SetColor(VGAColor fg, VGAColor bg);
	void Print(const char* str);
	void Puts(char c);

	//Prints a formatted string e.g. Print("Hello %?", "World!");
	template <typename T, typename... Targs>
	void Print(const char* fmt, T value, Targs... Fargs)
	{
		for (; *fmt; ++fmt) 
		{
			if (*fmt == '%' && *(fmt+1) == '?')
			{
				PrintVal(value);
				fmt += 2;
				Print(fmt, Fargs...);
				break;
			}
			else
				Puts(*fmt);
		}
	}

private:
	template <typename T> void PrintVal(T val)
	{ Print("???"); }
	void PrintVal(const char* s);
	void PrintVal(short value);
	void PrintVal(int value);
	void PrintVal(char value);
	void PrintVal(unsigned short value);
	void PrintVal(unsigned int value);

	int cursorX, cursorY;
	short color;
};

extern VGA vga;
