#include "ArrayIndexingExpr.h"

#include "errorHandling/semanticErrors/IllegalOpOnType.h"
#include "errorHandling/semanticErrors/IllegalTypeCast.h"

ArrayIndexingExpr::ArrayIndexingExpr(const Token& token, std::unique_ptr<Call> call, std::unique_ptr<Expr> index)
    : Call(token), call(std::move(call)) ,index(std::move(index))
{
}

std::unique_ptr<IType> ArrayIndexingExpr::getType() const
{
    const auto t = call->getType();
    if (!t) return nullptr;
    const auto arrT = t->getArrType();
    return arrT ? arrT->copy() : nullptr;
}

std::string ArrayIndexingExpr::translateToCpp() const
{
    std::string res = needsSemicolon ? getTabs() : "";
    res += call->translateToCpp() + "[" + index->translateToCpp() + "]";
    if (needsSemicolon) res += ";";
    return res;
}

void ArrayIndexingExpr::analyze() const
{
    if (symTable == nullptr) return;

    call->setSymbolTable(symTable);
    call->setScope(scope);
    call->setClassNode(currClass);
    call->analyze();

    index->setSymbolTable(symTable);
    index->setScope(scope);
    index->setClassNode(currClass);
    index->analyze();

    if (call->getType()->getArrLevel() == 0)
    {
        throw IllegalOpOnType(token, call->getType()->toString());
    }

    if (!index->getType()->isNumberable())
    {
        throw IllegalTypeCast(token, index->getType()->toString(), "degree");
    }
}

std::string ArrayIndexingExpr::toString() const
{
    return call->toString();
}
