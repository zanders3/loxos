#include "common.h"
#include "keyboard.h"
#include "isr.h"

bool g_keyboard_shift = false;
static const u32 NumKeyboardHandlers = 4;
static KeyboardHandler g_keyboardHandlers[NumKeyboardHandlers];
static u32 g_numKeyboardHandlers = 0;

static char getVal(const char* keyVals)
{
    if (g_keyboard_shift && keyVals[0] != '\0')
        return keyVals[1];
    else
        return keyVals[0];
}

static void keyboard_handler(const Registers&)
{
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
    case 0xE0:
        //ignore extended keys (for now)
        return;
    }

    if (keyValue == Key::LShift || keyValue == Key::RShift)
        g_keyboard_shift = keyDown;

    KeyInfo keyInfo = {
        keyValue,
        charValue,
        code,
        keyDown
    };
    for (u32 i = 0; i<g_numKeyboardHandlers; i++)
        g_keyboardHandlers[i](keyInfo);
}

void init_keyboard()
{
    register_irq_handler(1, &keyboard_handler);
}

void register_keyboard_handler(KeyboardHandler handler)
{
    g_keyboardHandlers[g_numKeyboardHandlers++] = handler;
    kassert(g_numKeyboardHandlers < NumKeyboardHandlers);
}
