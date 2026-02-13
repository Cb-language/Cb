#pragma once
#include "AST/abstract/Call.h"
#include "../../../symbols/Var.h"

class VarCallExpr : public Call
{
protected:
    const Var var;

public:
    VarCallExpr(const Token& token, Scope* scope, IFuncDeclStmt* funcDecl, const ClassNode* currClass, const Var& var);
    void analyze() const override;
    std::string translateToCpp() const override;
    std::unique_ptr<IType> getType() const override;
    std::wstring getName() const;
};
