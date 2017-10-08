#pragma once
#include "scanner.h"
#include "value.h"
#include "karray.h"
#include "kuniqueptr.h"

#define AST(rettype, parent, name, vars) struct parent##name;
#include "astdefs.h"
#undef AST

struct Visitor
{
    #define AST(rettype, parent, name, vars) virtual void Visit##name(const parent##name& val) = 0;
    #include "astdefs.h"
    #undef AST
};

struct Expr
{
    virtual void Visit(Visitor& visitor) const = 0;
};

struct Stmt
{
    virtual void Visit(Visitor& visitor) const = 0;
};

#define AST(rettype, parent, name, vars) struct parent##name : public parent { vars virtual rettype Visit(Visitor& visitor) const override { visitor.Visit##name(*this); } };
#include "astdefs.h"
#undef AST
