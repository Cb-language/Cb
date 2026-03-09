#pragma once
#include "AST/abstract/Call.h"
#include "../../../symbols/Var.h"

class VarCallExpr : public Call
{
protected:
    const Var var;

public:
    VarCallExpr(const Token& token, IFuncDeclStmt* funcDecl, const Var& var, ClassDeclStmt* classDecl = nullptr);
    void analyze() const override;
    std::string translateToCpp() const override;
    std::unique_ptr<IType> getType() const override;
    std::string getName() const;
    std::string toString() const override;
};
