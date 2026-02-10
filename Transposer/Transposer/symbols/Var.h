#pragma once
#include <memory>
#include <string>
#include "Type/Type.h"

class Var
{
private:
    std::unique_ptr<IType> type;
    const std::wstring name;

public:
    Var(std::unique_ptr<IType> type, const std::wstring& name);
    Var(const Var& other);
    std::unique_ptr<IType> getType() const;
    std::wstring getName() const;

    bool operator==(const Var& other) const;

    bool operator==(const std::wstring& other) const;

    bool isNumberable() const;
    bool isStringable() const;

    bool isPrimitive() const;

    Var copy() const;
};
