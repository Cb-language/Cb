#pragma once
#include "Expression.h"

class Call : public Expr
{
protected:
    explicit Call(const Token& token, Scope* scope, IFuncDeclStmt* funcDecl, ClassNode* currClass, const bool hasParens)
                    : Expr(token, scope, funcDecl, currClass, hasParens) {}

public:
    Call(const Token& token, Scope* scope, IFuncDeclStmt* funcDecl, ClassNode* currClass) : Expr(token, scope, funcDecl, currClass) {}
};