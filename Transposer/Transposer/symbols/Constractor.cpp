#include "Constractor.h"

Constractor::Constractor(const std::vector<Var>& args, const std::string& className) : className(className )
{
    for (const auto& arg : args) this->args.emplace_back(arg.copy());
}

const std::vector<Var>& Constractor::getArgs() const
{
    return args;
}

const std::string& Constractor::getClassName() const
{
    return className;
}

std::string Constractor::translateToCpp() const
{
    std::string res = className + "(";

    bool first = true;
    for (const auto& arg : args)
    {
        if (!first)
        {
            res += ", ";
        }
        res += arg.getType()->translateTypeToCpp() + " " + arg.getName();
        first = false;
    }

    res += ")";

    return res;
}

bool Constractor::operator==(const Constractor& other) const
{
    if (args.size() != other.args.size()) return false;

    for (int i = 0; i < args.size(); ++i)
    {
        if (args[i].getType() != other.args[i].getType()) return false;
    }

    return true;
}

bool Constractor::isLegalCall(const Constractor& other) const
{
    return *this == other;
}

Constractor Constractor::copy() const
{
    return Constractor(args, className);
}
