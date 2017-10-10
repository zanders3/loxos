#include "lox.h"
#include "scanner.h"
#include "parser.h"
#include "vga.h"

struct Interpreter : public Visitor
{
    Value VisitAssign(const ExprAssign&)
    {
        return Value();
    }

    bool IsEqual(const Value& left, const Value& right) const
    {
        if (left.type == ValueType::NIL && right.type == ValueType::NIL) return true;
        if (left.type == ValueType::NIL) return false;
        
        if (left.type == ValueType::STRING || right.type == ValueType::STRING)
        {
            if (left.type != ValueType::STRING || right.type != ValueType::STRING)
                return false;

            //TODO: string comparison, also better string handling, etc.
            return false;
        }

        return left.intValue == right.intValue;
    }

    bool CheckNumbers(const Token* op, const Value& left, const Value& right, bool error = true) const
    {
        if (left.type == ValueType::NUMBER && right.type == ValueType::NUMBER)
            return true;

        if (error)
            lox_error(*op, "Operands must be numbers");
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

                    const int leftLen = strlen(leftStr);
                    const int rightLen = strlen(rightStr);
                    SharedPtr<char> newStrPtr = SharedPtr<char>::Make(leftLen + rightLen + 1);
                    char* newStr = newStrPtr.Get();
                    for (int i = 0; i<leftLen; ++i)
                        newStr[i] = leftStr[i];
                    for (int i = 0; i<rightLen; ++i)
                        newStr[i+leftLen] = rightStr[i];
                    newStr[leftLen+rightLen] = '\0';
                    return Value(newStrPtr);
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

    Value VisitLogical(const ExprLogical& expr) 
    {
        vga.Print("[");
        expr.left->Visit(*this);
        vga.Print(" %? ", tokentype_to_string(expr.op->type));
        expr.right->Visit(*this);
        vga.Print("]");
        return Value();
    }
    
    Value VisitUnary(const ExprUnary& expr)
    {
        vga.Print("[%? ", tokentype_to_string(expr.op->type));
        expr.right->Visit(*this);
        vga.Print("]");
        return Value();
    }

    Value VisitVariable(const ExprVariable&) 
    {
        return Value();
    }

    void VisitBlock(const StmtBlock&) 
    {

    }

    void VisitExpression(const StmtExpression& expr) 
    {
        expr.expr->Visit(*this).Print();
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

    void VisitVar(const StmtVar&) 
    {

    }

    void VisitWhile(const StmtWhile&) 
    {

    }
};

void lox_run(const char* source, int sourceLen)
{
    Array<Token> tokens;
    scanner_scan(source, sourceLen, tokens);
    Array<UniquePtr<Stmt>> stmts;
    parser_parse(tokens, stmts);

    Interpreter interpreter;
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
