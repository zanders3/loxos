#pragma once

#define KEY(enumName, keyCodes, charValue) enumName,
enum class Key
{
    None,
    #include "keycodes.h"
    Count
};
#undef KEY

void keyboard_init();
const char* keyboard_readline();
