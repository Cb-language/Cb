#include "VarCallExpr.h"

VarCallExpr::VarCallExpr(const Token& token, Scope* scope, FuncDeclStmt* funcDecl, const Var& var) : Call(token, scope, funcDecl), var(var.copy())
{
}

bool VarCallExpr::isLegal() const
{
    return true;
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
