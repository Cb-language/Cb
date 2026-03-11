#include "VarCallExpr.h"

VarCallExpr::VarCallExpr(const Token& token, const Var& var)
    : Call(token), var(var.copy())
{
}

void VarCallExpr::analyze() const
{
}

std::string VarCallExpr::translateToCpp() const
{
    return translateFQNtoString(var.getName());
}

std::unique_ptr<IType> VarCallExpr::getType() const
{
    return var.getType()->copy();
}

std::string VarCallExpr::getName() const
{
    return translateFQNtoString(var.getName());
}

std::string VarCallExpr::toString() const
{
    return translateFQNtoString(var.getName());
}
