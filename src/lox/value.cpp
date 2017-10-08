#include "value.h"
#include "vga.h"

Value::Value()
    : type(ValueType::NIL)
{}
Value::Value(bool value)
    : type(ValueType::BOOL)
    , boolValue(value)
{}
Value::Value(int value)
    : type(ValueType::NUMBER)
    , intValue(value)
{}
Value::Value(const char* value)
    : type(ValueType::STRING)
    , stringValue(value)
{}

void Value::Print() const
{
    switch (type)
    {
        case ValueType::BOOL:
            vga.Print(boolValue ? "true" : "false");
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
