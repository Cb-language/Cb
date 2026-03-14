#pragma once
#include "AST/abstract/VarReference.h"
#include "AST/abstract/Expression.h"

enum class UnaryOp
{
    Zero = 0,
    PlusPlus,
    MinusMinus,
    PlusPlusPlusPlus,
    MinusMinusMinusMinus,
    Not
};

class UnaryOpExpr : public Expr
{
private:
    const std::unique_ptr<Expr> operand;
    const UnaryOp op;
public:
    UnaryOpExpr(const Token& token,
        std::unique_ptr<Expr> operand, const UnaryOp op, bool needsSemicolon = false);
    void analyze() const override;
    std::string translateToCpp() const override;
    std::unique_ptr<IType> getType() const override;
};