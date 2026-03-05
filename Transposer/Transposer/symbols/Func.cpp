#include "Func.h"

#include "other/Utils.h"

Func::Func(std::unique_ptr<IType> rType, const std::wstring& funcName, const std::vector<Var>& args, const VirtualType vType) : rType(std::move(rType)), funcName(funcName), virtualType(vType)
{
    for (const auto& arg : args)
    {
        this->args.emplace_back(arg.copy());
    }
}

Func::Func(const Func& other) : Func(other.rType->copy(), other.funcName, other.args, other.virtualType)
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

std::unique_ptr<IType> Func::getType() const
{
    return rType->copy();
}

void Func::setVirtual(const VirtualType vType)
{
    this->virtualType = vType;
}

VirtualType Func::getVirtual() const
{
    return virtualType;
}

std::string Func::translateToCpp(const std::wstring& className) const
{
    std::string funcNameStr = Utils::wstrToStr(funcName);
    std::string header;

    if (virtualType == VirtualType::Virtual || virtualType == VirtualType::Pure)
    {
        header += "virtual ";
    }

    header += rType->translateTypeToCpp() + " ";

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

    if (virtualType == VirtualType::Override)
    {
        header += " override";
    }

    if (virtualType == VirtualType::Pure)
    {
        header += " = 0";
    }

    return header;
}

bool Func::operator==(const Func& other) const
{
    if (rType->getType() != other.rType->getType())
    {
        return false;
    }

    return this->isSameNameAndArgs(other);
}

bool Func::operator!=(const Func& other) const
{
    return !(*this == other);
}

bool Func::isSameNameAndArgs(const Func& other) const
{
    if (funcName != other.funcName || args.size() != other.args.size())
    {
        return false;
    }

    for (size_t i = 0; i < args.size(); ++i)
    {
        if (*(args[i].getType()) != *(other.args[i].getType()))
        {
            return false;
        }
    }

    return true;
}

Func Func::copy() const
{
    return Func(*this);
}

bool Func::operator<(const Func& other) const
{
    return funcName < other.funcName;
}
