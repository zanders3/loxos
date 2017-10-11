#include "env.h"
#include "value.h"
#include "ast.h"
#include "lox.h"

Environment::Environment()
    : m_values(String::HashFunc)
{}

Value Environment::Get(const Token* token) const 
{
    if (m_values.Contains(token->stringLiteral))
        return m_values.Get(token->stringLiteral);
    lox_error(*token, "Undefined variable");
    return Value();
}

void Environment::Assign(const Token* token, const Value& value) 
{
    if (m_values.Contains(token->stringLiteral))
        m_values.Get(token->stringLiteral) = value;
    lox_error(*token, "Undefined variable");
}

void Environment::Define(const Token* token, const Value& value)
{
    m_values.Get(token->stringLiteral) = value;
}
