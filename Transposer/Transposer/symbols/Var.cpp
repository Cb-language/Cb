#include "Var.h"

#include <algorithm>

Var::Var(std::unique_ptr<IType> type, const FQN& name, const bool isStatic) : type(std::move(type)), name(name), isStatic(isStatic)
{
}

Var::Var(const Var& other) : Var(other.type ? other.type->copy() : nullptr, other.name, other.isStatic)
{
}

std::unique_ptr<IType> Var::getType() const
{
    return type ? type->copy() : nullptr;
}

const FQN& Var::getName() const
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
    if (this->type == nullptr || other.type == nullptr) return false;
    return this->name == other.name && *(this->type) == *(other.type) && this->isStatic == other.isStatic;
}

bool Var::operator==(const std::string& other) const
{
    if (type == nullptr) return false;
    return type->toString() == other;
}

void Var::operator=(const Var& other)
{
    this->type = other.type ? other.type->copy() : nullptr;
    this->name = other.name;
    this->isStatic = other.isStatic;
}

bool Var::isNumberable() const
{
    return type != nullptr && type->isNumberable();
}

bool Var::isStringable() const
{
    return type != nullptr && type->isStringable();
}

bool Var::isPrimitive() const
{
    return type != nullptr && type->isPrimitive();
}

void Var::setIsStatic(const bool isStatic)
{
    this->isStatic = isStatic;
}

Var Var::copy() const
{
    return Var(*this);
}
