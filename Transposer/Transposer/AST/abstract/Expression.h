#pragma once

#include "Statement.h"

class Expr : public Stmt
{
protected:
    bool hasParens;
    explicit Expr(const Token& token, Scope* scope, IFuncDeclStmt* funcDecl, const bool hasParens) : Stmt(token, scope, funcDecl), hasParens(hasParens) {}
public:
    Expr(const Token& token, Scope* scope, IFuncDeclStmt* funcDecl);
    ~Expr() override = default;
    virtual std::unique_ptr<IType> getType() const = 0;
    void analyze() const override = 0;
    std::string translateToCpp() const override = 0;

    virtual void setHasParens(const bool hasParens);
};

inline Expr::Expr(const Token& token, Scope* scope, IFuncDeclStmt* funcDecl) : Expr(token, scope, funcDecl, false)
{
}

inline void Expr::setHasParens(const bool hasParens)
{this->hasParens = hasParens;}
