#pragma once

void puts(char c);
void printval(const char* fmt);
void printval(int val);
void printval(unsigned int val);

inline void printval(short val) { printval((int)val); }
inline void printval(char val) { printval((int)val); }
inline void printval(unsigned short val) { printval((unsigned int)val); }
inline void printval(unsigned char val) { printval((unsigned int)val); }

inline void printf(const char* fmt) { printval(fmt); }
template <typename T, typename... Targs>
void printf(const char* fmt, T value, Targs... Fargs)
{
	for (; *fmt; ++fmt) 
	{
		if (*fmt == '%' && *(fmt+1) == '?')
		{
			printval(value);
			fmt += 2;
			printf(fmt, Fargs...);
			break;
		}
		else
			puts(*fmt);
	}
}
