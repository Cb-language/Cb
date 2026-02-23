#include "VarDeclStmt.h"

#include "errorHandling/semanticErrors/IllegalTypeCast.h"

VarDeclStmt::VarDeclStmt(const Token& token, Scope* scope, IFuncDeclStmt* funcDecl, const ClassNode* currClass, const bool hasStartingValue, std::unique_ptr<Expr> startingValue, const Var& var) :
    Stmt(token, scope, funcDecl, currClass), hasStartingValue(hasStartingValue), startingValue(std::move(startingValue)) , var(var.copy())
{
}

void VarDeclStmt::analyze() const
{
    if (hasStartingValue && *(var.getType()) != *(startingValue->getType()))
    {
        throw IllegalTypeCast(token, var.getType()->toString(), startingValue->getType()->toString());
    }

    if (startingValue != nullptr) startingValue->analyze();
}

std::string VarDeclStmt::translateToCpp() const
{
    std::string ret = getTabs() + var.getType()->translateTypeToCpp() + " " + Utils::wstrToStr(var.getName());

    if (hasStartingValue && startingValue != nullptr)
    {
        ret += " = " + var.getType()->translateTypeToCpp() + "(" + startingValue->translateToCpp() + ")";
    }

    ret += ";";
    return ret;
}

const Var& VarDeclStmt::getVar() const
{
    return var;
}
