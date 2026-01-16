#pragma once
#include "Expression.h"

class Call : public Expr
{
protected:
    explicit Call(Scope* scope, FuncDeclStmt* funcDecl, const bool hasParens) : Expr(scope, funcDecl, hasParens) {}

public:
    Call(Scope* scope, FuncDeclStmt* funcDecl) : Expr(scope, funcDecl) {}
};