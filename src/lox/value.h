#pragma once
#include "ksharedptr.h"

enum class ValueType
{
    NIL, BOOL, NUMBER, STRING
};

struct Value
{
    Value();
    Value(bool value);
    Value(int value);
    Value(const SharedPtr<char>& value);

    ValueType type;
    SharedPtr<char> stringValue;
    int intValue;

    void Print() const;
    int ToInt() const;
};
