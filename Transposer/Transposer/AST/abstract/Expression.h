#pragma once

#include "Statement.h"

class Expr : public Stmt
{
protected:
    bool hasParens;

    explicit Expr(const Token& token);

public:
    ~Expr() override = default;
    virtual std::unique_ptr<IType> getType() const = 0;
    void analyze() const override = 0;
    std::string translateToCpp() const override = 0;


    virtual void setHasParens(const bool hasParens);
    virtual void setNeedsSemicolon(const bool needsSemicolon);
};


inline Expr::Expr(const Token& token) : Stmt(token), hasParens(false)
{
}

inline void Expr::setHasParens(const bool hasParens)
{
    this->hasParens = hasParens;
}

inline void Expr::setNeedsSemicolon(const bool needsSemicolon)
{
}
