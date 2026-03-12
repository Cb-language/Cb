#include "ConstValueExpr.h"

#include <algorithm>

#include "errorHandling/how/HowDidYouGetHere.h"


std::string ConstValueExpr::getValueStr() const
{
    if (type->toString() == "freq" && value.starts_with("."))
    {
        return "0" + value;
    }

    if (type->toString() == "bar")
    {
        std::string content = value;
        std::ranges::replace(content, '\n', ' ');
        return content;
    }

    if (type->toString() == "mute")
    {
        return value == "cres" ? "true" : "false";
    }

    return value;
}

ConstValueExpr::ConstValueExpr(const Token& token, std::unique_ptr<IType> type, const std::string& value)
    : Expr(token), type(std::move(type)), value(value)
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
    std::ostringstream oss;
    oss << type->translateTypeToCpp() + "(";

    if (type->toString() == "bar") oss << "\"";
    if (type->toString() == "note") oss << "\'";

    oss << getValueStr();

    if (type->toString() == "bar") oss << "\"";
    if (type->toString() == "note") oss << "\'";

    oss << ")";
    return oss.str();
}

std::unique_ptr<IType> ConstValueExpr::getType() const
{
    return type->copy();
}

std::string ConstValueExpr::getValue() const
{
    return value;
}
