#include "VarCallExpr.h"

VarCallExpr::VarCallExpr(const Token& token, IFuncDeclStmt* funcDecl, const Var& var, ClassDeclStmt* classDecl)
    : Call(token, funcDecl, classDecl), var(var.copy())
{
}

void VarCallExpr::analyze() const
{
}

std::string VarCallExpr::translateToCpp() const
{
    return var.getName();
}

std::unique_ptr<IType> VarCallExpr::getType() const
{
    return var.getType()->copy();
}

std::string VarCallExpr::getName() const
{
    return var.getName();
}

std::string VarCallExpr::toString() const
{
    return var.getName();
}
