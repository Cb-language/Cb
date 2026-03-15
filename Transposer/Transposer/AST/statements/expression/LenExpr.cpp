#include "LenExpr.h"

#include "errorHandling/semanticErrors/IllegalLengthCall.h"

LenExpr::LenExpr(const Token& token, std::unique_ptr<VarReference> ref, const bool isNegative) : Expr(token), varRef(std::move(ref)), isNegative(isNegative)
{
}

std::unique_ptr<IType> LenExpr::getType() const
{
    return std::make_unique<PrimitiveType>(Primitive::TYPE_DEGREE);
}

void LenExpr::analyze() const
{
    varRef->analyze();

    if (varRef->getType()->getArrLevel() == 0) // non-riff types have 0
    {
        throw IllegalLengthCall(token, varRef->getType()->toString());
    }
}

std::string LenExpr::translateToCpp() const
{
    return varRef->translateToCpp() + "." + (isNegative ? "NegLength" : "Length") + "()";
}

void LenExpr::setSymbolTable(SymbolTable* symTable) const
{
    Stmt::setSymbolTable(symTable);
    varRef->setSymbolTable(symTable);
}
