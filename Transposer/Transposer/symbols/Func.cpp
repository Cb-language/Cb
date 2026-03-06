#include "Func.h"

#include "other/Utils.h"

Func::Func(std::unique_ptr<IType> rType, const std::wstring& funcName, const std::vector<Var>& args, const VirtualType vType, const ClassNode* owner) : rType(std::move(rType)), funcName(funcName), virtualType(vType), owner(owner)
{
    for (const auto& arg : args)
    {
        this->args.emplace_back(arg.copy());
    }
}

Func::Func(const Func& other) : Func(other.rType->copy(), other.funcName, other.args, other.virtualType, other.owner)
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

void Func::setOwner(const ClassNode* owner)
{
    this->owner = owner;
}

const ClassNode* Func::getOwner() const
{
    return owner;
}

std::string Func::translateToCpp(const std::wstring& className) const
{
    std::string funcNameStr = Utils::wstrToStr(funcName);
    std::string header;

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

    return header;
}

bool Func::operator==(const Func& other) const
{
    if (rType->getType() != other.rType->getType())
    {
        return false;
    }

    if (owner != other.owner)
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
    if (owner != other.owner)
    {
        return owner < other.owner;
    }

    if (funcName != other.funcName)
    {
        return funcName < other.funcName;
    }

    if (args.size() != other.args.size())
    {
        return args.size() < other.args.size();
    }

    for (size_t i = 0; i < args.size(); ++i)
    {
        if (args[i].getType()->getType() != other.args[i].getType()->getType())
        {
            return args[i].getType()->getType() < other.args[i].getType()->getType();
        }
    }

    return false;
}
