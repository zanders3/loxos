#include "lox.h"
#include "vga.h"

void lox_run(const char* line)
{
    vga.Print(line);
}
