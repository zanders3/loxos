#pragma once

enum class ValueType
{
    NIL, BOOL, NUMBER, STRING
};

struct Value
{
    Value();
    Value(bool value);
    Value(int value);
    Value(const char* value);

    ValueType type;
    const char* stringValue;
    int intValue;

    void Print() const;
    int ToInt() const;
};
