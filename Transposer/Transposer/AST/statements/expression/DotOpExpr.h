#pragma once
#include "BinaryOpExpr.h"
#include "AST/abstract/VarReference.h"

class DotOpExpr : public VarReference
{
private:
    std::unique_ptr<VarReference> left;
    std::unique_ptr<VarReference> right;

public:
    DotOpExpr(const Token& token,
        std::unique_ptr<VarReference> left, std::unique_ptr<VarReference> right);

    std::unique_ptr<IType> getType() const override;
    void analyze() const override;
    std::string translateToCpp() const override;
    std::string toString() const override;
    void setSymbolTable(SymbolTable* symTable) const override;
};
