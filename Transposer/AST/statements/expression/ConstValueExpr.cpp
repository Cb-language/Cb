#include "ConstValueExpr.h"

#include <algorithm>


ConstValueExpr::ConstValueExpr(Scope* scope, FuncDeclStmt* funcDecl, std::unique_ptr<IType> type, const std::wstring &value)
    : Expr(scope, funcDecl), type(std::move(type)), value(value)
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
    if (*type == L"freq" && value.starts_with(L"."))
    {
        return Utils::wstrToStr(L"0" + value);
    }

    if (*type == L"bar")
    {
        std::string content = Utils::wstrToStr(value);
        std::ranges::replace(content, '\n', ' ');
        return content;
    }

    if (*type == L"mute")
    {
        return value == L"cres" ? "true" : "false";
    }

    return Utils::wstrToStr(value);
}

std::unique_ptr<IType> ConstValueExpr::getType() const
{
    return type->copy();
}

std::wstring ConstValueExpr::getValue() const
{
    return value;
}
