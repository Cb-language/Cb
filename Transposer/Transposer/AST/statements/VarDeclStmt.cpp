#include "VarDeclStmt.h"

#include "errorHandling/semanticErrors/IllegalTypeCast.h"

VarDeclStmt::VarDeclStmt(const Token& token, Scope* scope, FuncDeclStmt* funcDecl, const bool hasStartingValue, std::unique_ptr<Expr> startingValue, const Var& var) :
    Stmt(token, scope, funcDecl), hasStartingValue(hasStartingValue), startingValue(std::move(startingValue)) , var(var.copy())
{
}

void VarDeclStmt::analyze() const
{
    if (hasStartingValue && *(var.getType()) != *(startingValue->getType()))
    {
        throw IllegalTypeCast(token, var.getType()->toString(), startingValue->getType()->toString());
    }
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