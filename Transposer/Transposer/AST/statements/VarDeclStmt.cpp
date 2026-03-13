#include "VarDeclStmt.h"

#include "errorHandling/semanticErrors/IllegalTypeCast.h"
#include "errorHandling/classErrors/IllegalFieldName.h"
#include "../../errorHandling/semanticErrors/IllegalVarName.h"
#include "other/SymbolTable.h"
#include "other/Utils.h"

VarDeclStmt::VarDeclStmt(const Token& token, const bool hasStartingValue, std::unique_ptr<Expr> startingValue, const Var& var) :
    Stmt(token), hasStartingValue(hasStartingValue), startingValue(std::move(startingValue)) , var(var.copy())
{
}

void VarDeclStmt::analyze() const
{
    if (symTable == nullptr) return;
    const std::string varNameStr = translateFQNtoString(var.getName());
    const bool startsWithNote = Utils::startsWithNote(varNameStr);

    if (symTable->getCurrClass() == nullptr)
    {
        // local variable
        if (startsWithNote) throw IllegalVarName(token);
        symTable->addVar(var, token);
    }
    else
    {
        // Field (should already be registered in Pass 2, but let's check name here)
        if (!startsWithNote) throw IllegalFieldName(token);
    }

    if (hasStartingValue)
    {
        startingValue->setSymbolTable(symTable);
        startingValue->setScope(symTable->getCurrScope());
        startingValue->setClassNode(symTable->getCurrClass());
        startingValue->analyze();
        
        if (translateFQNtoString(var.getType()->getFQN()) != translateFQNtoString(startingValue->getType()->getFQN()))
        {
            throw IllegalTypeCast(token, var.getType()->toString(), startingValue->getType()->toString());
        }
    }
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
