#include "LenExpr.h"

#include "errorHandling/semanticErrors/IllegalLengthCall.h"

LenExpr::LenExpr(const Token& token, std::unique_ptr<Call> call, const bool isNegative) : Expr(token), call(std::move(call)), isNegative(isNegative)
{
}

std::unique_ptr<IType> LenExpr::getType() const
{
    return std::make_unique<PrimitiveType>(Primitive::TYPE_DEGREE);
}

void LenExpr::analyze() const
{
    call->analyze();

    if (call->getType()->getArrLevel() == 0) // non-riff types have 0
    {
        throw IllegalLengthCall(token, call->getType()->toString());
    }
}

std::string LenExpr::translateToCpp() const
{
    return call->translateToCpp() + "." + (isNegative ? "NegLength" : "Length") + "()";
}
