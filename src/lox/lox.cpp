#include "lox.h"
#include "scanner.h"
#include "parser.h"
#include "vga.h"

struct PrintVisitor : public Visitor
{
    void VisitAssign(const ExprAssign&)
    {
        vga.Print("ASSIGN");
    }

    void VisitBinary(const ExprBinary& expr)
    {
        vga.Print("[");
        expr.left->Visit(*this);
        vga.Print(" %? ", tokentype_to_string(expr.op->type));
        expr.right->Visit(*this);
        vga.Print("]");
    }

    void VisitCall(const ExprCall&) {}

    void VisitGrouping(const ExprGrouping& group)
    {
        vga.Print("(");
        group.expr->Visit(*this);
        vga.Print(")");
    }

    void VisitLiteral(const ExprLiteral& lit) 
    {
        lit.value.Print();
    }

    void VisitLogical(const ExprLogical& expr) 
    {
        vga.Print("[");
        expr.left->Visit(*this);
        vga.Print(" %? ", tokentype_to_string(expr.op->type));
        expr.right->Visit(*this);
        vga.Print("]");
    }
    
    void VisitUnary(const ExprUnary& expr)
    {
        vga.Print("[%? ", tokentype_to_string(expr.op->type));
        expr.right->Visit(*this);
        vga.Print("]");
    }

    void VisitVariable(const ExprVariable&) {}

    void VisitBlock(const StmtBlock&) {}
    void VisitExpression(const StmtExpression& expr) 
    {
        expr.expr->Visit(*this);
    }

    void VisitFunction(const StmtFunction&) {}
    void VisitIf(const StmtIf&) {}
    void VisitPrint(const StmtPrint&) {}
    void VisitReturn(const StmtReturn&) {}
    void VisitVar(const StmtVar&) {}
    void VisitWhile(const StmtWhile&) {}
};

void lox_run(const char* source, int sourceLen)
{
    Array<Token> tokens;
    scanner_scan(source, sourceLen, tokens);
    Array<UniquePtr<Stmt>> stmts;
    parser_parse(tokens, stmts);

    PrintVisitor printVisitor;
    for (const UniquePtr<Stmt>& stmt : stmts)
    {
        if (stmt.IsValid())
            stmt->Visit(printVisitor);
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
        vga.Print("[line %?] Error %? at end", token.line, message);
    else
        vga.Print("[line %?] Error %? at %?", token.line, message, token.lexeme);
    vga.SetColor(VGAColor::White, VGAColor::Black);
}
