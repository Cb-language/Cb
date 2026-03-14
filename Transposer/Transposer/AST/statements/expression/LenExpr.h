#pragma once
#include "AST/abstract/Call.h"
#include "AST/abstract/Expression.h"

class LenExpr : public Expr
{
private:
    std::unique_ptr<Call> call;
    const bool isNegative;

public:
    LenExpr(const Token& token, std::unique_ptr<Call> call, const bool isNegative = false);

    std::unique_ptr<IType> getType() const override;
    void analyze() const override;
    std::string translateToCpp() const override;
};
