#pragma once

#include "../../Statements.h"

enum class UnaryOp {
    Zero = 0,
    PlusPlus,
    MinusMinus
};

class UnaryOperatorExpr : public Expr
{
private:
    const std::unique_ptr<VarCallExpr> var;
    const UnaryOp op;
public:
    UnaryOperatorExpr(std::unique_ptr<VarCallExpr>, const UnaryOp op);
    bool isLegal() const override;
    std::string translateToCpp() const override;
    Type getType() const override;
};