#include "value.h"
#include "vga.h"
#include "common.h"

Value::Value()
    : type(ValueType::NIL)
    , stringValue(nullptr)
    , intValue(0)
{}
Value::Value(bool value)
    : type(ValueType::BOOL)
    , stringValue(nullptr)
    , intValue(value)
{}
Value::Value(int value)
    : type(ValueType::NUMBER)
    , stringValue(nullptr)
    , intValue(value)
{}
Value::Value(const char* value)
    : type(ValueType::STRING)
    , stringValue(value)
    , intValue(0)
{}

void Value::Print() const
{
    switch (type)
    {
        case ValueType::BOOL:
            vga.Print(intValue ? "true" : "false");
            break;
        case ValueType::NUMBER:
            vga.Print("%?", intValue);
            break;
        case ValueType::STRING:
            vga.Print(stringValue);
            break;
        case ValueType::NIL:
            vga.Print("nil");
            break;
    }
}
