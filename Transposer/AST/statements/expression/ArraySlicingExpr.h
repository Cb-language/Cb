#pragma once
#include "VarCallExpr.h"

class ArraySlicingExpr : public VarCallExpr
{
private:
    const int start;
    const int stop;
    const int step;
public:
    ArraySlicingExpr(Scope* scope, FuncDeclStmt* funcDecl, const Var& var, const int start, const int stop, const int step);
    ArraySlicingExpr(const ArraySlicingExpr& other);
    bool isLegal() const override;
    std::string translateToCpp() const override;
    std::unique_ptr<IType> getType() const override;
};
