#include "CastCallExpr.h"

#include "errorHandling/how/HowDidYouGetHere.h"
#include "errorHandling/semanticErrors/IllegalTypeCast.h"

CastCallExpr::CastCallExpr(const Token& token, std::unique_ptr<Expr> expr, std::unique_ptr<IType> type) : VarReference(token), expr(std::move(expr)), type(std::move(type))
{
}

std::unique_ptr<IType> CastCallExpr::getType() const
{
    return type->copy();
}

void CastCallExpr::analyze() const
{
    const auto exprType = expr->getType()->copy();

    if (exprType == nullptr) throw HowDidYouGetHere(token);
    expr->analyze();
    if (exprType == type || type == exprType) return;
    throw IllegalTypeCast(token, translateFQNtoString(exprType->getFQN()), translateFQNtoString(type->getFQN()));
}

std::string CastCallExpr::translateToCpp() const
{
    return "Utils::cast<" + type->translateTypeToCpp() + ">(SafePtr<Object>(" + expr->translateToCpp() + "))";
}

std::string CastCallExpr::toString() const
{
    return translateToCpp();
}
