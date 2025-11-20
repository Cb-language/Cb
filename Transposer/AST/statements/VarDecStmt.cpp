#include "VarDecStmt.h"

VarDecStmt::VarDecStmt(const bool hasStartingValue, const Type type, std::unique_ptr<Expr> startingValue) : hasStartingValue(hasStartingValue), type(type), startingValue(std::move(startingValue))
{
}

bool VarDecStmt::isLegal() const
{
    if (hasStartingValue)
    {
        return (type == startingValue->getType());
    }
    return true;
}

std::string VarDecStmt::translateToCpp() const
{
}

Type VarDecStmt::Gettype() const
{
    return type;
}
