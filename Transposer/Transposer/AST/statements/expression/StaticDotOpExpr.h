#pragma once
#include "AST/abstract/VarReference.h"
#include "symbols/Type/ClassType.h"

class StaticDotOpExpr : public VarReference
{
private:
    std::unique_ptr<ClassType> left;
    std::unique_ptr<VarReference> right;

public:
    StaticDotOpExpr(const Token& token, std::unique_ptr<ClassType> left, std::unique_ptr<VarReference> right, const bool needsSemicolon = false);

    std::unique_ptr<IType> getType() const override;
    void analyze() const override;
    std::string translateToCpp() const override;
    std::string toString() const override;
    void setSymbolTable(SymbolTable* symTable) const override;
};
