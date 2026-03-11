#include "VarDeclStmt.h"

#include "errorHandling/semanticErrors/IllegalTypeCast.h"

VarDeclStmt::VarDeclStmt(const Token& token, IFuncDeclStmt* funcDecl, const bool hasStartingValue, std::unique_ptr<Expr> startingValue, const Var& var, ClassDeclStmt* classDecl) :
    Stmt(token, funcDecl, classDecl), hasStartingValue(hasStartingValue), startingValue(std::move(startingValue)) , var(var.copy())
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
    std::string prefix = "";
    if (var.getStatic()) prefix = "static ";
    std::string ret = getTabs() + prefix + var.getType()->translateTypeToCpp() + " " + translateFQNtoString(var.getName());

    if (hasStartingValue && startingValue != nullptr && !var.getStatic())
    {
        ret += " = ";
        if (var.getType()->toString() != startingValue->getType()->toString()) ret += var.getType()->translateTypeToCpp() + "(" + startingValue->translateToCpp() + ")";
        else ret +=  startingValue->translateToCpp();
    }

    ret += ";";
    return ret;
}

const Var& VarDeclStmt::getVar() const
{
    return var;
}

const Expr* VarDeclStmt::getStartingValue() const
{
    return startingValue.get();
}

void VarDeclStmt::setIsStatic(const bool isStatic)
{
    this->var.setIsStatic(isStatic);
}
