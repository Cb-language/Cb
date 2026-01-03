#include "ArrayIndexingExpr.h"

ArrayIndexingExpr::ArrayIndexingExpr(const Token& token, Scope* scope, FuncDeclStmt* funcDecl, std::unique_ptr<Call> call, std::unique_ptr<Expr> index)
    : Call(token, scope, funcDecl), call(std::move(call)) ,index(std::move(index))
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

bool ArrayIndexingExpr::isLegal() const
{
    return call->getType()->getArrLevel() > 0 && index->isLegal() && index->getType()->isNumberable();
}
