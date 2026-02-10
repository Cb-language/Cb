#include "VarCallExpr.h"

VarCallExpr::VarCallExpr(const Token& token, Scope* scope, IFuncDeclStmt* funcDecl, const Var& var) : Call(token, scope, funcDecl), var(var.copy())
{
}

void VarCallExpr::analyze() const
{
}

std::string VarCallExpr::translateToCpp() const
{
    return Utils::wstrToStr(var.getName());
}

std::unique_ptr<IType> VarCallExpr::getType() const
{
    return var.getType()->copy();
}

std::wstring VarCallExpr::getName() const
{
    return var.getName();
}
