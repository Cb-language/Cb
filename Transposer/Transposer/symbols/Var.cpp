#include "Var.h"

#include <algorithm>

Var::Var(std::unique_ptr<IType> type, const std::wstring& name, bool isStatic) : type(std::move(type)), name(name), isStatic(isStatic)
{
}

Var::Var(const Var& other) : Var(other.type->copy(), other.name, other.isStatic)
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

bool Var::getStatic() const
{
    return isStatic;
}

void Var::setStatic(bool isStatic)
{
    this->isStatic = isStatic;
}

bool Var::operator==(const Var& other) const
{
    return this->name == other.name && *(this->type) == *(other.type) && this->isStatic == other.isStatic;
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
    return Var(*this);
}
