#pragma once
#include "AST/abstract/VarReference.h"
#include "AST/abstract/Expression.h"

class LenExpr : public Expr
{
private:
    std::unique_ptr<VarReference> varRef;
    const bool isNegative;

public:
    LenExpr(const Token& token, std::unique_ptr<VarReference> ref, const bool isNegative = false);

    std::unique_ptr<IType> getType() const override;
    void analyze() const override;
    std::string translateToCpp() const override;
};
