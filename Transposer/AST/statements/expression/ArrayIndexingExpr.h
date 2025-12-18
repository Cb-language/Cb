#pragma once
#include "VarCallExpr.h"

class ArrayIndexingExpr : public VarCallExpr
{
private:
    std::unique_ptr<Expr> index;

public:
    ArrayIndexingExpr(Scope* scope, FuncDeclStmt* funcDecl, const Var& var, std::unique_ptr<Expr> index);

    std::unique_ptr<IType> getType() const override;
    std::string translateToCpp() const override;
    bool isLegal() const override;
};
