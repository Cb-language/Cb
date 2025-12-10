#include "FuncCall.h"

#include <sstream>

#include "other/Utils.h"

FuncCall::FuncCall(const std::wstring& name, const std::vector<std::wstring>& args, const Token& token)
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

const Type& FuncCall::getType() const
{
    return type;
}

void FuncCall::setType(const Type& type)
{
    this->type = type;
}

std::string FuncCall::translateToCpp() const
{
    std::ostringstream oss;

    oss << Utils::wstrToStr(name) << "(";

    for (const std::wstring& arg : args)
    {
        oss << Utils::wstrToStr(arg);
    }
    oss << ")";

    return oss.str();
}
