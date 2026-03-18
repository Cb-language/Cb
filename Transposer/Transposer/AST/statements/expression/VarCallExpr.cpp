#include "VarCallExpr.h"
#include "other/SymbolTable.h"
#include "errorHandling/semanticErrors/UnrecognizedIdentifier.h"

VarCallExpr::VarCallExpr(const Token& token, const Var& var)
    : VarReference(token), var(var.copy())
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
        symTable->addError(std::make_unique<UnrecognizedIdentifier>(token, translateFQNtoString(var.getName())));
    }
}

std::string VarCallExpr::translateToCpp() const
{
    std::string res = (needsSemicolon && !isClassItem) ? getTabs() : "";
    res += translateFQNtoString(var.getName());
    if (needsSemicolon) res += ";";
    return res;
}

std::unique_ptr<IType> VarCallExpr::getType() const
{
    const auto t = var.getType();
    return t ? t->copy() : nullptr;
}

const FQN& VarCallExpr::getName() const
{
    return var.getName();
}

std::string VarCallExpr::toString() const
{
    return translateFQNtoString(var.getName());
}
