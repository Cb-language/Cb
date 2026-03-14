#include "IsExpr.h"

IsExpr::IsExpr(const Token& token, std::unique_ptr<Expr> expr, std::unique_ptr<IType> type) : Expr(token), expr(std::move(expr)), type(std::move(type))
{
}

std::unique_ptr<IType> IsExpr::getType() const
{
    return std::make_unique<PrimitiveType>(Primitive::TYPE_MUTE);
}

void IsExpr::analyze() const
{
    expr->analyze();
}

std::string IsExpr::translateToCpp() const
{
    return "Utils::is<" + type->translateTypeToCpp() + ">(SafePtr<Object>(" + expr->translateToCpp() + "))";
}
