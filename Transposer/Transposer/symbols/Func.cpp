#include "Func.h"

#include "other/Utils.h"

Func::Func(std::unique_ptr<IType> rType, const std::wstring& funcName, const std::vector<Var>& args) : rType(std::move(rType)), funcName(funcName)
{
    for (const auto& arg : args)
    {
        this->args.emplace_back(arg.copy());
    }
}

const std::vector<Var>& Func::getArgs() const
{
    return args;
}

const std::wstring& Func::getFuncName() const
{
    return funcName;
}

std::unique_ptr<IType> Func::getType() const
{
    return rType->copy();
}

std::string Func::translateToCpp(const std::wstring& className) const
{
    std::string funcNameStr = Utils::wstrToStr(funcName);

    if (funcNameStr == "prelude")
    {
        funcNameStr = "main";
    }

    std::string header = rType->translateTypeToCpp() + " ";

    if (!className.empty())
    {
        header += Utils::wstrToStr(className) + "::";
    }

    header += funcNameStr + "(";

    bool first = true;
    for (const auto& arg : args)
    {
        if (!first)
        {
            header += ", ";
        }
        header += arg.getType()->translateTypeToCpp() + " " + Utils::wstrToStr(arg.getName());
        first = false;
    }

    header += ")";
    return header;
}

bool Func::operator==(const Func& other) const
{
    if (rType->getType() != other.rType->getType() || funcName != other.funcName)
    {
        return false;
    }

    for (const auto& arg : other.args)
    {
        // arg name doesn't matter for func
        if (*(arg.getType()) != rType->getType())
        {
            return false;
        }
    }

    return true;
}

bool Func::isLegalCall(const Func& other) const
{
    if (funcName != other.funcName)
    {
        return false;
    }

    for (const auto& arg : other.args)
    {
        // arg name doesn't matter for func
        if (*(arg.getType()) != rType->getType())
        {
            return false;
        }
    }

    return true;
}

Func Func::copy() const
{
    return Func(rType->copy(), funcName, args);
}

bool Func::operator<(const Func& other) const
{
    return funcName < other.funcName;
}
