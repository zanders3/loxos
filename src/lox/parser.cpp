#include "parser.h"
#include "lox.h"

struct Parser
{
    const Array<Token>& m_tokens;
    int m_current;

    Parser(const Array<Token>& tokens)
        : m_tokens(tokens)
        , m_current(0)
    {}

    inline const Token& Peek() { return m_tokens[m_current]; }
    inline const Token& Previous() { return m_tokens[m_current - 1]; }
    inline bool IsAtEnd() { return Peek().type == TokenType::EOF; }

    const Token& Advance()
    {
        if (!IsAtEnd()) ++m_current;
        return Previous();
    }

    bool Check(TokenType type)
    {
        if (IsAtEnd()) return false;
        return Peek().type == type;
    }

    bool Match(TokenType type)
    {
        if (Check(type))
        {
            Advance();
            return true;
        }
        return false;
    }

    const Token* Consume(TokenType type, const char* message)
    {
        if (Check(type)) return &Advance();

        lox_error(Peek(), message);
        return nullptr;
    }

    void Synchronize()
    {
        Advance();

        while (!IsAtEnd())
        {
            if (Previous().type == TokenType::SEMICOLON) 
                return;
            switch (Peek().type)
            {
                case TokenType::CLASS: case TokenType::FUN: case TokenType::VAR:
                case TokenType::IF: case TokenType::WHILE: case TokenType::PRINT: case TokenType::RETURN:
                    return;
                default:
                    Advance();
                    break;
            }
        }
    }

    // program -> declaration* EOF
    void Parse(Array<UniquePtr<Stmt>>& stmts)
    {
        while (!IsAtEnd())
        {
            UniquePtr<Stmt> stmt(Declaration());
            if (stmt.IsValid())
                stmts.Add((UniquePtr<Stmt>&&)stmt);
        }
    }

    //declaration -> funcDecl | varDecl | statement
    Stmt* Declaration()
    {
        Stmt* stmt = nullptr;
        //if (Match(TokenType::FUN)) stmt = Function();
        if (Match(TokenType::VAR)) stmt = VarDecl();
        else stmt = Statement();

        if (stmt == nullptr)
            Synchronize();

        return stmt;
    }

    // statement -> exprStmt | forStmt | ifStmt | printStmt | returnStmt | whileStmt | block
    Stmt* Statement()
    {
        if (Match(TokenType::PRINT)) return PrintStatement();

        return ExpressionStatement();
    }

    // printStmt -> "print" expression ";"
    Stmt* PrintStatement()
    {
        Expr* expr = Expression();
        if (expr == nullptr)
            return nullptr;
        if (!Consume(TokenType::SEMICOLON, "Expect ';' after expression"))
        {
            FreeExpr(expr);
            return nullptr;
        }
        
        StmtPrint* stmt = kalloc<StmtPrint>();
        stmt->expr.Assign(expr);
        return stmt;
    }

    // varDecl -> "var" IDENTIFIER ( "=" expression )? ";"
    Stmt* VarDecl()
    {
        const Token* name = Consume(TokenType::IDENTIFIER, "Expected variable name");
        if (name == nullptr)
            return nullptr;

        Expr* initializer = nullptr;
        if (Match(TokenType::EQUAL))
            initializer = Expression();

        if (!Consume(TokenType::SEMICOLON, "Expect ';' after variable declaration"))
        {
            if (initializer)
                FreeExpr(initializer);
            return nullptr;
        }

        StmtVar* stmt = kalloc<StmtVar>();
        stmt->name = name;
        if (initializer)
            stmt->initializer.Assign(initializer);
        return stmt;
    }

    // exprStmt -> expression ";"
    Stmt* ExpressionStatement()
    {
        Expr* expr = Expression();
        if (expr == nullptr)
            return nullptr;
        if (!Consume(TokenType::SEMICOLON, "Expect ';' after expression"))
        {
            FreeExpr(expr);
            return nullptr;
        }
        
        StmtExpression* stmt = kalloc<StmtExpression>();
        stmt->expr.Assign(expr);
        return stmt;
    }

    // expression -> assignment
    Expr* Expression()
    {
        return Assignment();
    }

    // assignment -> identifier "=" assignment
    //             | logic_or
    Expr* Assignment()
    {
        Expr* expr = LogicOr();
        if (expr == nullptr)
            return nullptr;
        
        if (Match(TokenType::EQUAL))
        {
            const Token& equals = Previous();
            Expr* value = Assignment();
            if (value == nullptr)
            {
                FreeExpr(expr);
                return nullptr;
            }

            if (expr->type == ASTType::ExprVariable)
            {
                const Token* name = ((ExprVariable*)expr)->name;
                ExprAssign* assignExpr = kalloc<ExprAssign>();
                assignExpr->name = name;
                assignExpr->value.Assign(value);
                FreeExpr(expr);
                return assignExpr;
            }

            lox_error(equals, "Invalid assignment target");
            FreeExpr(expr);
            FreeExpr(value);
            return nullptr;
        }
        return expr;
    }

    void FreeExpr(Expr*& expr)
    {
        kassert(expr);
        expr->~Expr();
        kfree(expr);
        expr = nullptr;
    }

    // logic_or -> logic_and ( "or" logic_or )*
    Expr* LogicOr()
    {
        Expr* expr = LogicAnd();
        if (expr == nullptr)
            return nullptr;

        while (Match(TokenType::OR))
        {
            const Token& op = Previous();
            Expr* right = LogicOr();
            if (right == nullptr)
            {
                FreeExpr(expr);
                return nullptr;
            }

            ExprLogical* newExpr = kalloc<ExprLogical>();
            newExpr->left.Assign(expr);
            newExpr->op = &op;
            newExpr->right.Assign(right);
            expr = newExpr;
        }

        return expr;
    }

    // logic_and -> equality ( "and" equality )*
    Expr* LogicAnd()
    {
        Expr* expr = Equality();
        if (expr == nullptr)
            return nullptr;

        while (Match(TokenType::AND))
        {
            const Token& op = Previous();
            Expr* right = Equality();
            if (right == nullptr)
            {
                FreeExpr(expr);
                return nullptr;
            }

            ExprLogical* newExpr = kalloc<ExprLogical>();
            newExpr->left.Assign(expr);
            newExpr->op = &op;
            newExpr->right.Assign(right);
            expr = newExpr;
        }

        return expr;
    }

    // equality -> comparison ( ( "!=" | "==" ) comparison )*
    Expr* Equality()
    {
        Expr* expr = Comparison();
        if (expr == nullptr)
            return nullptr;

        while (Match(TokenType::BANG_EQUAL) || Match(TokenType::EQUAL_EQUAL))
        {
            const Token& op = Previous();
            Expr* right = Comparison();
            if (right == nullptr)
            {
                FreeExpr(expr);
                return nullptr;
            }
            ExprBinary* exprBin = kalloc<ExprBinary>();
            exprBin->left.Assign(expr);
            exprBin->op = &op;
            exprBin->right.Assign(right);
            expr = exprBin;
        }

        return expr;
    }

    //comparison → addition ( ( ">" | ">=" | "<" | "<=" ) addition )*
    Expr* Comparison()
    {
        Expr* expr = Addition();
        if (expr == nullptr)
            return nullptr;

        while (Match(TokenType::GREATER) || Match(TokenType::GREATER_EQUAL) || Match(TokenType::LESS) || Match(TokenType::LESS_EQUAL))
        {
            const Token& op = Previous();
            Expr* right = Addition();
            if (right == nullptr)
            {
                FreeExpr(expr);
                return nullptr;
            }
            ExprBinary* exprBin = kalloc<ExprBinary>();
            exprBin->left.Assign(expr);
            exprBin->op = &op;
            exprBin->right.Assign(right);
            expr = exprBin;
        }

        return expr;
    }

    //addition       → multiplication ( ( "-" | "+" ) multiplication )*
    Expr* Addition()
    {
        Expr* expr = Multiplication();
        if (expr == nullptr)
            return nullptr;

        while (Match(TokenType::MINUS) || Match(TokenType::PLUS))
        {
            const Token& op = Previous();
            Expr* right = Multiplication();
            if (right == nullptr)
            {
                FreeExpr(expr);
                return nullptr;
            }
            ExprBinary* exprBin = kalloc<ExprBinary>();
            exprBin->left.Assign(expr);
            exprBin->op = &op;
            exprBin->right.Assign(right);
            expr = exprBin;
        }

        return expr;
    }

    //multiplication → unary ( ( "/" | "*" ) unary )*
    Expr* Multiplication()
    {
        Expr* expr = Unary();
        if (expr == nullptr)
            return nullptr;

        while (Match(TokenType::SLASH) || Match(TokenType::STAR))
        {
            const Token& op = Previous();
            Expr* right = Unary();
            if (right == nullptr)
            {
                FreeExpr(expr);
                return nullptr;
            }
            ExprBinary* exprBin = kalloc<ExprBinary>();
            exprBin->left.Assign(expr);
            exprBin->op = &op;
            exprBin->right.Assign(right);
            expr = exprBin;
        }

        return expr;
    }

    //unary          → ( "!" | "-" ) unary
    //               | call
    Expr* Unary()
    {
        if (Match(TokenType::BANG) || Match(TokenType::MINUS))
        {
            const Token& op = Previous();
            Expr* right = Unary();
            if (right == nullptr)
                return nullptr;
            ExprUnary* exprUn = kalloc<ExprUnary>();
            exprUn->op = &op;
            exprUn->right.Assign(right);
            return exprUn;
        }

        return Call();
    }

    //call -> primary ( "(" arguments? ")" )*
    Expr* Call()
    {
        return Primary();
        //TODO: function call
    }

    //primary        → NUMBER | STRING | "false" | "true" | "nil"
    //               | "(" expression ")" ;
    //               | IDENTIFIER
    Expr* Primary()
    {
        if (Match(TokenType::FALSE)) return Literal(Value(false));
        if (Match(TokenType::TRUE)) return Literal(Value(true));
        if (Match(TokenType::NIL)) return Literal(Value());

        if (Match(TokenType::NUMBER))
            return Literal(Value(Previous().numberLiteral));
        if (Match(TokenType::STRING))
            return Literal(Value(Previous().stringLiteral));

        if (Match(TokenType::LEFT_PAREN))
        {
            Expr* expr = Expression();
            if (expr == nullptr)
                return nullptr;
            if (!Consume(TokenType::RIGHT_PAREN, "Expect ')' after expression"))
                return nullptr;
            ExprGrouping* exprGroup = kalloc<ExprGrouping>();
            exprGroup->expr.Assign(expr);
            return exprGroup;
        }

        if (Match(TokenType::IDENTIFIER))
        {
            ExprVariable* var = kalloc<ExprVariable>();
            var->name = &Previous();
            return var;
        }

        lox_error(Peek(), "Expect expression");
        return nullptr;
    }

    Expr* Literal(const Value& value)
    {
        ExprLiteral* expr = kalloc<ExprLiteral>();
        expr->value = value;
        return expr;
    }
};

void parser_parse(const Array<Token>& tokens, Array<UniquePtr<Stmt>>& stmts)
{
    Parser parser(tokens);
    parser.Parse(stmts);
}
