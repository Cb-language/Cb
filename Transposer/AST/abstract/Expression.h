#pragma once

#include "Statement.h"
#include "../../symbols/Type.h"

class Expr : public Stmt
{
protected:
    bool hasParens;
    explicit Expr(Scope* scope, FuncDeclStmt* funcDecl, const bool hasParens) : Stmt(scope, funcDecl), hasParens(hasParens) {}
public:
    Expr(Scope* scope, FuncDeclStmt* funcDecl);
    ~Expr() override = default;
    virtual Type getType() const = 0;
    bool isLegal() const override = 0;
    std::string translateToCpp() const override = 0;

    void setHasParens(const bool hasParens);
};

inline Expr::Expr(Scope* scope, FuncDeclStmt* funcDecl) : Expr(scope, funcDecl, false)
{
}

inline void Expr::setHasParens(const bool hasParens)
{this->hasParens = hasParens;}
