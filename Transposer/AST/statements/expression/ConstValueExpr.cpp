#include "ConstValueExpr.h"


ConstValueExpr::ConstValueExpr(const Type &type, const std::wstring &value) : type(type), value(value)
{
}

bool ConstValueExpr::isLegal() const
{
    if (value.empty())
    {
        return false;
    }
    // literal is always legal
    return true;
}

std::string ConstValueExpr::translateToCpp() const
{
    if (type == L"freq" && value.starts_with(L"."))
    {
        return Utils::wstrToStr(L"0" + value);
    }

    if (type == L"bar")
    {
        const std::string content = Utils::wstrToStr(value);
        return "R\"(" + content.substr(1, content.size() - 2) + ")\""; // removes the \" at the start and the end
    }

    return Utils::wstrToStr(value);
}

Type ConstValueExpr::getType() const
{
    return type;
}
