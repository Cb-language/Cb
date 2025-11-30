#include "VarDecStmt.h"

VarDecStmt::VarDecStmt(Scope* scope, FuncDeclStmt* funcDecl, const bool hasStartingValue, std::unique_ptr<Expr> startingValue, const Var& var) :
    Stmt(scope, funcDecl), hasStartingValue(hasStartingValue), startingValue(std::move(startingValue)) , var(var)
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
    std::string ret = getTabs() + var.getType().translateTypeToCpp() + " " + Utils::wstrToStr(var.getName());

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