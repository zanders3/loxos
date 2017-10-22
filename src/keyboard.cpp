#include "keyboard.h"
#include "idt.h"
#include "print.h"

bool g_keyboard_shift = false;
char g_keyboardBuffer[80];
int  g_bufIdx = 0;
bool g_completedLine = true;

static char getVal(const char* keyVals)
{
    if (g_keyboard_shift && keyVals[0] != '\0')
        return keyVals[1];
    else
        return keyVals[0];
}

const char* keyboard_readline()
{
    g_completedLine = false;
    g_bufIdx = 0;
    g_keyboardBuffer[g_bufIdx] = '\0';
    while (!g_completedLine) asm("hlt");
    return g_keyboardBuffer;
}

__attribute__((interrupt)) void keyboard_handler(int*)
{
    idt_irq_ack(1);
    u8 code = inb(0x60);
    char charValue = '\0';
    Key keyValue = Key::None;
    bool keyDown = false;
    switch (code)
    {
        #define KEY(enumName, keyCode, charStr) case keyCode: { keyValue = Key::enumName; charValue = getVal(charStr); keyDown = true; } break;
        #include "keycodes.h"
        #undef KEY
        #define KEY(enumName, keyCode, charStr) case (keyCode+0x80): { keyValue = Key::enumName; charValue = getVal(charStr); keyDown = false; } break;
        #include "keycodes.h"
        #undef KEY
    case 0xE0://ignore extended keys
        return;
    }

    if (keyValue == Key::LShift || keyValue == Key::RShift)
        g_keyboard_shift = keyDown;

    if (!keyDown || g_completedLine)
        return;
    
    if (g_bufIdx > 0 && keyValue == Key::Backspace)
    {
        g_keyboardBuffer[g_bufIdx--] = '\0';
        puts(0x10);
    }
    else if (charValue >= ' ' && charValue <= '~' && g_bufIdx < 78)
    {
        g_keyboardBuffer[g_bufIdx++] = charValue;
        puts(charValue);
    }
    else if (keyValue == Key::Enter)
    {
        g_keyboardBuffer[g_bufIdx] = '\n';
        g_keyboardBuffer[g_bufIdx+1] = '\0';
        g_completedLine = true;
        puts('\n');
    }
}

void keyboard_init()
{
    idt_irq_setup(1, (u32)&keyboard_handler);
}
