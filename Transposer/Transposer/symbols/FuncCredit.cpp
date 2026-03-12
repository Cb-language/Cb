#include "FuncCredit.h"

FuncCredit::FuncCredit(const FQN& name, const Token& token) : name(name), token(token)
{
}

FuncCredit::FuncCredit(const FuncCredit& funcCredit) : name(funcCredit.name), token(funcCredit.token)
{
}

const Token& FuncCredit::getToken() const
{
    return token;
}

const FQN& FuncCredit::getName() const
{
    return name;
}

bool FuncCredit::isLegalCredit(const Func& func) const
{
    return func.getFuncName() == name;
}
