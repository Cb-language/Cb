#pragma once
#include "AST/abstract/VarReference.h"
#include "../../../symbols/Var.h"

class VarCallExpr : public VarReference
{
protected:
    Var var;

public:
    VarCallExpr(const Token& token, const Var& var);
    void analyze() const override;
    std::string translateToCpp() const override;
    std::unique_ptr<IType> getType() const override;
    std::string getName() const;
    std::string toString() const override;
};
