#include "FuncCall.h"

FuncCall::FuncCall(const std::wstring& name, const std::vector<Var>& args, const Token& token)
    : name(name), args(args), token(token)
{
}

FuncCall::FuncCall(const std::wstring& name, const Token& token) : name(name), args(), token(token)
{
}

FuncCall::FuncCall(const FuncCall& other) : name(other.name), args(other.args), token(other.token)
{
}

bool FuncCall::isLegalCall(const Func& other) const
{
    if (name != other.getFuncName() || args.size() != other.getArgs().size())
    {
        return false;
    }

    for (int i = 0; i < args.size(); i++)
    {
        if (args[i] != other.getArgs()[i])
        {
            return false;
        }
    }

    return true;
}

const Token& FuncCall::getToken() const
{
    return token;
}
