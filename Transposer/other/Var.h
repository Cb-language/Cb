#pragma once
#include <string>
#include "Type.h"

class Var
{
private:
    const Type type;
    const std::wstring name;

public:
    Var(const Type& type, const std::wstring& name);
    Type getType() const;
    std::wstring getName() const;

    bool operator==(const Var& other) const;

    bool operator==(const std::wstring& other) const;

    bool isNumberable() const;
    bool isStringable() const;

    bool isPrimitive() const;
};
