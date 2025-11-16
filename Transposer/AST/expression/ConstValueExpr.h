#pragma once

#include "AST/abstract/Expression.h"

class ConstValueExpr : public Expr
{
private:
    Type type;
    std::string value;
public:
    ConstValueExpr(const int line, const int column, const Type &type, const std::string &value);
    Type getType() const override;
    bool isLegal() const override;
    std::string print() const override;
};