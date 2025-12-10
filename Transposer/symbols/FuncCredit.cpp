#include "FuncCredit.h"

FuncCredit::FuncCredit(const std::wstring& name, const Token& token) : name(name), token(token)
{
}

FuncCredit::FuncCredit(const FuncCredit& funcCredit) : name(funcCredit.name), token(funcCredit.token)
{
}

const Token& FuncCredit::getToken() const
{
    return token;
}

bool FuncCredit::isLegalCredit(const Func& func) const
{
    return func.getFuncName() == name;
}
