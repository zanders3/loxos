#include "lox.h"
#include "scanner.h"
#include "parser.h"
#include "env.h"
#include "vga.h"
#include "std/kstring.h"

struct Interpreter : public Visitor
{
    Interpreter(Environment& env)
        : environment(env)
    {}

    bool IsEqual(const Value& left, const Value& right) const
    {
        if (left.type == ValueType::NIL && right.type == ValueType::NIL) return true;
        if (left.type == ValueType::NIL) return false;
        
        if (left.type == ValueType::STRING || right.type == ValueType::STRING)
        {
            if (left.type != ValueType::STRING || right.type != ValueType::STRING)
                return false;

            return left.stringValue == right.stringValue;
        }

        return left.intValue == right.intValue;
    }

    static bool CheckNumbers(const Token* op, const Value& left, const Value& right, bool error = true)
    {
        if (left.type == ValueType::NUMBER && right.type == ValueType::NUMBER)
            return true;

        if (error)
            lox_error(*op, "Operands must be numbers");
        return false;
    }

    static bool CheckNumbers(const Token* op, const Value& operand)
    {
        if (operand.type == ValueType::NUMBER) return true;
        lox_error(*op, "Operand must be a number");
        return false;
    }

    Value VisitBinary(const ExprBinary& expr)
    {
        Value left = expr.left->Visit(*this);
        Value right = expr.right->Visit(*this);
        switch (expr.op->type)
        {
            case TokenType::MINUS:
                if (CheckNumbers(expr.op, left, right))
                    return left.intValue - right.intValue;
                break;
            case TokenType::PLUS:
                if (CheckNumbers(expr.op, left, right, false))
                    return left.intValue + right.intValue;
                else if (left.type == ValueType::STRING)
                {
                    const char* leftStr = left.stringValue.Get();

                    const char* rightStr = nullptr;
                    char rightStrBuf[20];
                    switch (right.type)
                    {
                        case ValueType::NIL: return left;
                        case ValueType::BOOL: rightStr = right.intValue ? "true" : "false"; break;
                        case ValueType::NUMBER: 
                            itoa(right.intValue, rightStrBuf, 10);
                            rightStr = &rightStrBuf[0];
                            break;
                        case ValueType::STRING: rightStr = right.stringValue.Get(); break;
                    }

                    return Value(String::Join(leftStr, rightStr));
                }
                break;
            case TokenType::STAR:
                if (CheckNumbers(expr.op, left, right))
                    return left.intValue * right.intValue;
                break;
            case TokenType::GREATER:
                if (CheckNumbers(expr.op, left, right))
                    return Value(left.intValue > right.intValue);
                break;
            case TokenType::GREATER_EQUAL:
                if (CheckNumbers(expr.op, left, right))
                    return Value(left.intValue >= right.intValue);
                break;
            case TokenType::LESS:
                if (CheckNumbers(expr.op, left, right))
                    return Value(left.intValue < right.intValue);
                break;
            case TokenType::LESS_EQUAL:
                if (CheckNumbers(expr.op, left, right))
                    return Value(left.intValue <= right.intValue);
                break;
            case TokenType::BANG_EQUAL:
                return !IsEqual(left, right);
            case TokenType::EQUAL_EQUAL:
                return IsEqual(left, right);
            default:
                lox_error(*expr.op, "Unknown operand");
                break;
        }
        return Value();
    }

    Value VisitCall(const ExprCall&) 
    {
        return Value();
    }

    Value VisitGrouping(const ExprGrouping& group)
    {
        return group.expr->Visit(*this);
    }

    Value VisitLiteral(const ExprLiteral& lit) 
    {
        return lit.value;
    }

    static bool IsTruthy(const Value& val)
    {
        if (val.type == ValueType::NIL) return false;
        if (val.type == ValueType::STRING) return true;
        return val.intValue > 0;
    }

    Value VisitLogical(const ExprLogical& expr) 
    {
        Value left = expr.left->Visit(*this);
        if (expr.op->type == TokenType::OR)
        {
            if (IsTruthy(left)) return left;
        }
        else
        {
            if (!IsTruthy(left)) return left;
        }
        
        return expr.right->Visit(*this);
    }
    
    Value VisitUnary(const ExprUnary& expr)
    {
        Value right = expr.right->Visit(*this);
        switch (expr.op->type)
        {
            case TokenType::MINUS:
                if (CheckNumbers(expr.op, right))
                    return -right.intValue;
            case TokenType::BANG:
                return !IsTruthy(right);
            default:
                break;
        }
        return Value();
    }

    void VisitExpression(const StmtExpression& expr) 
    {
        expr.expr->Visit(*this);
    }

    Value VisitVariable(const ExprVariable& expr) 
    {
        return environment.Get(expr.name);
    }

    Value VisitAssign(const ExprAssign& expr)
    {
        Value value = expr.value->Visit(*this);
        environment.Assign(expr.name, value);
        return value;
    }

    void VisitVar(const StmtVar& stmt)
    {
        Value value;
        if (stmt.initializer.IsValid())
            value = stmt.initializer->Visit(*this);
        environment.Define(stmt.name, value);
    }

    void VisitBlock(const StmtBlock&) 
    {

    }

    void VisitFunction(const StmtFunction&) 
    {

    }

    void VisitIf(const StmtIf&) 
    {

    }

    void VisitPrint(const StmtPrint& expr) 
    {
        expr.expr->Visit(*this).Print();
    }

    void VisitReturn(const StmtReturn&) 
    {

    }

    void VisitWhile(const StmtWhile&) 
    {

    }
private:
    Environment& environment;
};

void lox_run(Environment& environment, const char* source, int sourceLen)
{
    Array<Token> tokens;
    scanner_scan(source, sourceLen, tokens);
    Array<UniquePtr<Stmt>> stmts;
    parser_parse(tokens, stmts);

    Interpreter interpreter(environment);
    for (const UniquePtr<Stmt>& stmt : stmts)
    {
        if (stmt.IsValid())
            stmt->Visit(interpreter);
    }
    vga.Puts('\n');
}

void lox_error(int line, const char* message)
{
    vga.SetColor(VGAColor::Red, VGAColor::Black);
    vga.Print("[line %?] Error %?\n", line, message);
    vga.SetColor(VGAColor::White, VGAColor::Black);
}

void lox_error(const Token& token, const char* message)
{
    vga.SetColor(VGAColor::Red, VGAColor::Black);
    if (token.type == TokenType::EOF)
        vga.Print("[line %?] Error %? at end\n", token.line, message);
    else
        vga.Print("[line %?] Error %? at %?\n", token.line, message, token.lexeme);
    vga.SetColor(VGAColor::White, VGAColor::Black);
}
