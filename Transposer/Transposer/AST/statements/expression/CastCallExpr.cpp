#include "CastCallExpr.h"

#include <algorithm>

#include "errorHandling/how/HowDidYouGetHere.h"
#include "errorHandling/semanticErrors/IllegalTypeCast.h"
#include "other/SymbolTable.h"

CastCallExpr::CastCallExpr(const Token& token, std::unique_ptr<Expr> expr, std::unique_ptr<IType> type)
: VarReference(token), expr(std::move(expr)), type(std::move(type))
{}

std::unique_ptr<IType> CastCallExpr::getType() const
{
    return type->copy();
}

void CastCallExpr::analyze() const
{
    expr->analyze();
    const auto exprType = expr->getType()->copy();

    if (exprType == nullptr) symTable->addError(std::make_unique<HowDidYouGetHere>((token)));
    if (*exprType == *type || *type == *exprType) return;
    symTable->addError(std::make_unique<IllegalTypeCast>(token, translateFQNtoString(exprType->getFQN()), translateFQNtoString(type->getFQN())));
}

std::string CastCallExpr::translateToCpp() const
{
    return "Utils::cast<" + type->translateTypeToCpp() + ", " + expr->getType()->translateTypeToCpp() + ">(" + expr->translateToCpp() + ")";
}

std::string CastCallExpr::toString() const
{
    return translateToCpp();
}

void CastCallExpr::setSymbolTable(SymbolTable* symTable) const
{
    Stmt::setSymbolTable(symTable);
    expr->setSymbolTable(symTable);
}
