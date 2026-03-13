#include "VarCallExpr.h"
#include "other/SymbolTable.h"
#include "errorHandling/semanticErrors/UnrecognizedIdentifier.h"

VarCallExpr::VarCallExpr(const Token& token, const Var& var)
    : Call(token), var(var.copy())
{
}

void VarCallExpr::analyze() const
{
    if (symTable == nullptr) return;

    if (const auto resolvedVar = symTable->getVar(var.getName(), currClass))
    {
        const_cast<VarCallExpr*>(this)->var = resolvedVar->copy();
    }
    else
    {
        throw UnrecognizedIdentifier(token, translateFQNtoString(var.getName()));
    }
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
