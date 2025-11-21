#include "VarCallExpr.h"

VarCallExpr::VarCallExpr(Scope* scope, FuncDeclStmt* funcDecl, const Var& var) : Expr(scope, funcDecl), var(var)
{
}

VarCallExpr::VarCallExpr(const VarCallExpr& other) : Expr(other.scope, other.funcDecl), var(other.var)
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

Type VarCallExpr::getType() const
{
    return var.getType();
}

std::wstring VarCallExpr::getName() const
{
    return var.getName();
}
