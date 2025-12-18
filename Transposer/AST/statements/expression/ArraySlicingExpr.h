#pragma once
#include "VarCallExpr.h"

class ArraySlicingExpr : public VarCallExpr
{
private:
    std::unique_ptr<Expr> start;
    std::unique_ptr<Expr> stop;
    std::unique_ptr<Expr> step;
public:
    ArraySlicingExpr(Scope* scope, FuncDeclStmt* funcDecl, const Var& var, std::unique_ptr<Expr> start, std::unique_ptr<Expr> stop, std::unique_ptr<Expr> step);
    ArraySlicingExpr(const ArraySlicingExpr& other);
    bool isLegal() const override;
    std::string translateToCpp() const override;
    std::unique_ptr<IType> getType() const override;
};
