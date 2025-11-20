#pragma once
#include "AST/abstract/Expression.h"
#include "other/Var.h"

class VarCallExpr : public Expr
{
private:
    const Var var;

public:
    VarCallExpr(const Var& var);
    bool isLegal() const override;
    std::string translateToCpp() const override;
    Type getType() const override;
    std::wstring getName() const;
};
