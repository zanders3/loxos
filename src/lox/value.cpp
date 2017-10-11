#include "value.h"
#include "vga.h"
#include "common.h"

Value::Value()
    : type(ValueType::NIL)
    , intValue(0)
{}
Value::Value(bool value)
    : type(ValueType::BOOL)
    , intValue(value)
{}
Value::Value(int value)
    : type(ValueType::NUMBER)
    , intValue(value)
{}
Value::Value(const String& value)
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
            vga.Print(stringValue.Get());
            break;
        case ValueType::NIL:
            vga.Print("nil");
            break;
    }
}
