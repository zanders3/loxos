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
    union {
        bool boolValue;
        int intValue;
        const char* stringValue;
    };

    void Print() const;
};
