#pragma once
#include "AST/abstract/VarReference.h"

class ArrayIndexingExpr : public VarReference
{
private:
    std::unique_ptr<VarReference> varRef;
    std::unique_ptr<Expr> index;

public:
    ArrayIndexingExpr(const Token& token, std::unique_ptr<VarReference> ref, std::unique_ptr<Expr> index);

    std::unique_ptr<IType> getType() const override;
    std::string translateToCpp() const override;
    void analyze() const override;
    std::string toString() const override;
};
