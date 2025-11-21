#include "Var.h"

Var::Var(const Type& type, const std::wstring& name) : type(type), name(name)
{
}

Type Var::getType() const
{
    return type;
}

std::wstring Var::getName() const
{
    return name;
}

bool Var::operator==(const Var& other) const
{
    return this->name == other.name && this->type == other.type;
}

bool Var::operator==(const std::wstring& other) const
{
    return this->type == other;
}

bool Var::isNumberable() const
{
    return *this == L"degree";
}

bool Var::isStringable() const
{
    return *this == L"bar";
}

bool Var::isPrimitive() const
{
    return isNumberable() || isStringable();
}
