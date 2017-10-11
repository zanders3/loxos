#pragma once
#include "std/kstring.h"
#include "std/kdictionary.h"
#include "lox/value.h"

class Token;

class Environment
{
public:
    Environment();
    Value Get(const Token* name) const;
    void Assign(const Token* name, const Value& value);
    void Define(const Token* name, const Value& value);

private:
    Dictionary<String,Value> m_values;
};
