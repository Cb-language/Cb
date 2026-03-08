#include "ConstValueExpr.h"

#include <algorithm>

#include "errorHandling/how/HowDidYouGetHere.h"


std::string ConstValueExpr::getValueStr() const
{
    if (*type == "freq" && value.starts_with("."))
    {
        return "0" + value;
    }

    if (*type == "bar")
    {
        std::string content = value;
        std::ranges::replace(content, '\n', ' ');
        return content;
    }

    if (*type == "mute")
    {
        return value == "cres" ? "true" : "false";
    }

    return value;
}

ConstValueExpr::ConstValueExpr(const Token& token, Scope* scope, IFuncDeclStmt* funcDecl, const ClassNode* currClass, std::unique_ptr<IType> type, const std::string &value)
    : Expr(token, scope, funcDecl, currClass), type(std::move(type)), value(value)
{
}

void ConstValueExpr::analyze() const
{
    if (value.empty())
    {
        throw HowDidYouGetHere(token);
    }
}

std::string ConstValueExpr::translateToCpp() const
{
    return type->translateTypeToCpp() + "(" + getValueStr() + ")";
}

std::unique_ptr<IType> ConstValueExpr::getType() const
{
    return type->copy();
}

std::string ConstValueExpr::getValue() const
{
    return value;
}
