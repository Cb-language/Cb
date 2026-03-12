#include "ArrayIndexingExpr.h"

#include "errorHandling/semanticErrors/IllegalOpOnType.h"
#include "errorHandling/semanticErrors/IllegalTypeCast.h"

ArrayIndexingExpr::ArrayIndexingExpr(const Token& token, std::unique_ptr<Call> call, std::unique_ptr<Expr> index)
    : Call(token), call(std::move(call)) ,index(std::move(index))
{
}

std::unique_ptr<IType> ArrayIndexingExpr::getType() const
{
    return call->getType()->getArrType()->copy();
}

std::string ArrayIndexingExpr::translateToCpp() const
{
    return call->translateToCpp() + "[" + index->translateToCpp() + "]";
}

void ArrayIndexingExpr::analyze() const
{
    if (call->getType()->getArrLevel() == 0)
    {
        throw IllegalOpOnType(token, call->getType()->toString());
    }

    if (!index->getType()->isNumberable())
    {
        throw IllegalTypeCast(token, index->getType()->toString(), "degree");
    }

    index->analyze();
}

std::string ArrayIndexingExpr::toString() const
{
    return call->toString();
}
