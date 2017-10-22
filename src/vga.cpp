const int Width = 80;
const int Height = 25;
const int Color = 0x0F00;
int g_cursorY = 0, g_cursorX = 0;

void vga_puts(char c)
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
