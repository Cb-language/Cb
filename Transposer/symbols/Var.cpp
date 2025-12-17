#include "Var.h"

Var::Var(std::unique_ptr<IType> type, const std::wstring& name) : type(std::move(type)), name(name)
{
}

std::unique_ptr<IType> Var::getType() const
{
    return type->copy();
}

std::wstring Var::getName() const
{
    return name;
}

bool Var::operator==(const Var& other) const
{
    return this->name == other.name && *(this->type) == *(other.type);
}

bool Var::operator==(const std::wstring& other) const
{
    return *type == other;
}

bool Var::isNumberable() const
{
    return type->isNumberable();
}

bool Var::isStringable() const
{
    return type->isStringable();
}

bool Var::isPrimitive() const
{
    return type->isPrimitive();
}

Var Var::copy() const
{
    return Var(type->copy(), name);
}
