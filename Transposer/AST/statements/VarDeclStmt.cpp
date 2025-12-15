#include "VarDeclStmt.h"

VarDeclStmt::VarDeclStmt(Scope* scope, FuncDeclStmt* funcDecl, const bool hasStartingValue, std::unique_ptr<Expr> startingValue, const Var& var) :
    Stmt(scope, funcDecl), hasStartingValue(hasStartingValue), startingValue(std::move(startingValue)) , var(var.copy())
{
}

bool VarDeclStmt::isLegal() const
{
    if (hasStartingValue)
    {
        return (var.getType() == startingValue->getType());
    }
    return true;
}

std::string VarDeclStmt::translateToCpp() const
{
    if (var.getType()->getType() == L"bar" && hasStartingValue && startingValue->getType()->getType() != L"bar")
    {
        return getTabs() + var.getType()->translateTypeToCpp() + " " + Utils::wstrToStr(var.getName())
        + " = std::to_string(" + startingValue->translateToCpp() + ");";
    }

    std::string ret = getTabs() + var.getType()->translateTypeToCpp() + " " + Utils::wstrToStr(var.getName());

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