#pragma once
#include "AST/abstract/Expression.h"
#include "../../../symbols/Var.h"

class VarCallExpr : public Expr
{
private:
    const Var var;

public:
    VarCallExpr(Scope* scope, FuncDeclStmt* funcDecl, const Var& var);
    VarCallExpr(const VarCallExpr& other);
    bool isLegal() const override;
    std::string translateToCpp() const override;
    std::unique_ptr<IType> getType() const override;
    std::wstring getName() const;
};
