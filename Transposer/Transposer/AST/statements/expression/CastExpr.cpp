#include "CastExpr.h"

#include "errorHandling/how/HowDidYouGetHere.h"
#include "errorHandling/semanticErrors/IllegalTypeCast.h"

CastExpr::CastExpr(const Token& token, std::unique_ptr<Expr> expr, std::unique_ptr<IType> type) : Expr(token), expr(std::move(expr)), type(std::move(type))
{
}

std::unique_ptr<IType> CastExpr::getType() const
{
    return type->copy();
}

void CastExpr::analyze() const
{
    const auto exprType = expr->getType()->copy();

    if (exprType == nullptr) throw HowDidYouGetHere(token);

    if (exprType == type || type == exprType) return;
    throw IllegalTypeCast(token, translateFQNtoString(exprType->getFQN()), translateFQNtoString(type->getFQN()));
}

std::string CastExpr::translateToCpp() const
{
    return "Utils::cast<" + type->translateTypeToCpp() + ">(SafePtr<Object>(" + expr->translateToCpp() + "))";
}
