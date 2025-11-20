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
    std::string ret = var.getType().translateTypeToCpp() + " " + Utils::wstrToStr(var.getName()) + " ";

    if (startingValue == nullptr)
    {
        ret += "= " + startingValue->translateToCpp();
    }
    ret += ";";
    return ret;
}

Type VarDecStmt::Gettype() const
{
    return var.getType();
}
