#pragma once

#include "Statement.h"

class Expr : public Stmt
{
protected:
    bool hasParens;
    explicit Expr(const Token& token, Scope* scope, IFuncDeclStmt* funcDecl, const ClassNode* currClass, const bool hasParens) : Stmt(token, scope, funcDecl, currClass),
                    hasParens(hasParens) {}
public:
    Expr(const Token& token, Scope* scope, IFuncDeclStmt* funcDecl, const ClassNode* currClass);
    ~Expr() override = default;
    virtual std::unique_ptr<IType> getType() const = 0;
    void analyze() const override = 0;
    std::string translateToCpp() const override = 0;

    virtual void setHasParens(const bool hasParens);
    virtual void setIsStmt(const bool isStmt);
};

inline Expr::Expr(const Token& token, Scope* scope, IFuncDeclStmt* funcDecl, const ClassNode* currClass) : Expr(token, scope, funcDecl, currClass, false)
{
}

inline void Expr::setHasParens(const bool hasParens)
{this->hasParens = hasParens;}

inline void Expr::setIsStmt(const bool isStmt)
{
}
