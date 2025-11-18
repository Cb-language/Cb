//
// Created by Cyber_User on 18/11/2025.
//

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
