#pragma once
#include "std/kstring.h"

enum class ValueType
{
    NIL, BOOL, NUMBER, STRING
};

struct Value
{
    Value();
    Value(bool value);
    Value(int value);
    Value(const String& value);

    ValueType type;
    String stringValue;
    int intValue;

    void Print() const;
    int ToInt() const;
};
