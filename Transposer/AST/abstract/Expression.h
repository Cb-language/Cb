#pragma once

#include "Statement.h"
#include "other/Type.h"

class Expr : public Stmt
{
protected:
    bool hasParens;
    explicit Expr(const bool hasParens) : hasParens(hasParens) {}
public:
    Expr() : Expr(false) {}
    ~Expr() override = default;
    virtual Type getType() const = 0;
    bool isLegal() const override = 0;
    std::string translateToCpp() const override = 0;

    void setHasParens(const bool hasParens);
};

inline void Expr::setHasParens(const bool hasParens)
{this->hasParens = hasParens;}
