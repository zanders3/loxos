#pragma once

#define KEY(enumName, keyCodes, charValue) enumName,
enum class Key
{
    None,
    #include "keycodes.h"
};
#undef KEY

struct KeyInfo
{
    Key key;
    char charValue;
    u8 scanCode;
    bool keyDown;
};

typedef void (*KeyboardHandler)(const KeyInfo& keyInfo);

void init_keyboard();
void register_keyboard_handler(KeyboardHandler handler);
