#include "Func.h"

#include "other/Utils.h"

Func::Func(const Type& rType, const std::wstring& funcName, const std::vector<Var>& args) : type(rType), funcName(funcName), args(args)
{
}

const std::vector<Var>& Func::getArgs() const
{
    return args;
}

const std::wstring& Func::getFuncName() const
{
    return funcName;
}

const Type& Func::getType() const
{
    return rType;
}

std::string Func::translateToCpp() const
{
    std::string header = Utils::printTabs()
    + rType.translateTypeToCpp() + " " + Utils::wstrToStr(funcName)
    + "(";

    bool first = true;
    for (const auto& arg : args)
    {
        if (!first)
        {
            header += ", ";
        }
        header += arg.getType().translateTypeToCpp() + " " + Utils::wstrToStr(arg.getName());
        first = false;
    }

    header += ")";
    return header;
}

bool Func::operator==(const Func& other) const
{
    if (rType.getType() != other.rType.getType() || funcName != other.funcName)
    {
        return false;
    }

    for (const auto& arg : other.args)
    {
        // arg name doesn't matter for func
        if (arg.getType() != rType.getType())
        {
            return false;
        }
    }

    return true;
}
