#pragma once
#include "AST/abstract/Call.h"
#include "../../../symbols/Var.h"

class VarCallExpr : public Call
{
protected:
    const Var var;

public:
    VarCallExpr(Scope* scope, FuncDeclStmt* funcDecl, const Var& var);
    bool isLegal() const override;
    std::string translateToCpp() const override;
    std::unique_ptr<IType> getType() const override;
    std::wstring getName() const;
};
