#pragma once

#include "Statement.h"

class Expr : public Stmt
{
protected:
    bool hasParens = false;
    bool needsSemicolon;

    explicit Expr(const Token& token);

public:
    ~Expr() override = default;
    virtual std::unique_ptr<IType> getType() const = 0;
    
    virtual void setHasParens(const bool hasParens);
    virtual void setNeedsSemicolon(const bool needsSemicolon);
};


inline Expr::Expr(const Token& token) : Stmt(token), hasParens(false), needsSemicolon(false)
{
}

inline void Expr::setHasParens(const bool hasParens)
{
    this->hasParens = hasParens;
}

inline void Expr::setNeedsSemicolon(const bool needsSemicolon)
{
    this->needsSemicolon = needsSemicolon;
}
