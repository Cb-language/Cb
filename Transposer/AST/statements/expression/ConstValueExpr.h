#pragma once

#include "AST/abstract/Expression.h"

class ConstValueExpr : public Expr
{
private:
    Type type;
    std::wstring value;
public:
    ConstValueExpr(const int line, const int column, const Type &type, const std::string &value);
    bool isLegal() const override;
    std::string translateToCpp() const override;
    Type getType() const override;
};