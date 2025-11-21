#include "VarDecStmt.h"

VarDecStmt::VarDecStmt(const bool hasStartingValue, std::unique_ptr<Expr> startingValue, const Var& var) :
    hasStartingValue(hasStartingValue), startingValue(std::move(startingValue)) , var(var)
{
}

bool VarDecStmt::isLegal() const
{
    if (hasStartingValue)
    {
        return (var.getType() == startingValue->getType());
    }
    return true;
}

std::string VarDecStmt::translateToCpp() const
{
    std::string ret = Utils::printTabs() + var.getType().translateTypeToCpp() + " " + Utils::wstrToStr(var.getName());

    if (hasStartingValue)
    {
        ret += " = " + startingValue->translateToCpp();
    }
    else if (var.isNumberable())
    {
        ret += " = 0";
    }

    ret += ";";
    return ret;
}