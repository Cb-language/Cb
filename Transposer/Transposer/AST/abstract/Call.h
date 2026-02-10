#pragma once
#include "Expression.h"

class Call : public Expr
{
protected:
    explicit Call(const Token& token, Scope* scope, IFuncDeclStmt* funcDecl, const bool hasParens) : Expr(token, scope, funcDecl, hasParens) {}

public:
    Call(const Token& token, Scope* scope, IFuncDeclStmt* funcDecl) : Expr(token, scope, funcDecl) {}
};