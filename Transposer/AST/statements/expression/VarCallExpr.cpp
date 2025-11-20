#include "VarCallExpr.h"

VarCallExpr::VarCallExpr(const Var& var) : var(var)
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
