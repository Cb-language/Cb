#include "ConstValueExpr.h"

#include <algorithm>

#include "errorHandling/how/HowDidYouGetHere.h"


ConstValueExpr::ConstValueExpr(const Token& token, Scope* scope, IFuncDeclStmt* funcDecl, ClassNode* currClass, std::unique_ptr<IType> type, const std::wstring &value)
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
