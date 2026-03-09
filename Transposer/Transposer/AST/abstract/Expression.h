#pragma once

#include "Statement.h"

class Expr : public Stmt
{
protected:
    bool hasParens;

    explicit Expr(const Token& token, ClassDeclStmt* classDecl = nullptr);
    Expr(const Token& token, IFuncDeclStmt* funcDecl, ClassDeclStmt* classDecl = nullptr);

public:
    ~Expr() override = default;
    virtual std::unique_ptr<IType> getType() const = 0;
    void analyze() const override = 0;
    std::string translateToCpp() const override = 0;


    virtual void setHasParens(const bool hasParens);
    virtual void setNeedsSemicolon(const bool needsSemicolon);
};


inline Expr::Expr(const Token& token, IFuncDeclStmt* funcDecl, ClassDeclStmt* classDecl) : Stmt(token, funcDecl,
    classDecl), hasParens(false)
{
}

inline Expr::Expr(const Token& token, ClassDeclStmt* classDecl) : Stmt(token, classDecl), hasParens(false)
{
}

inline void Expr::setHasParens(const bool hasParens)
{
    this->hasParens = hasParens;
}

inline void Expr::setNeedsSemicolon(const bool needsSemicolon)
{
}
