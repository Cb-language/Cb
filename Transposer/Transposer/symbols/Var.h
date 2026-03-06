#pragma once
#include <memory>
#include <string>
#include "Type/Type.h"

class Var
{
private:
    std::unique_ptr<IType> type;
    const std::wstring name;
    bool isStatic;

public:
    Var(std::unique_ptr<IType> type, const std::wstring& name, bool isStatic = false);
    Var(const Var& other);
    std::unique_ptr<IType> getType() const;
    std::wstring getName() const;
    bool getStatic() const;
    void setStatic(bool isStatic);

    bool operator==(const Var& other) const;

    bool operator==(const std::wstring& other) const;

    bool isNumberable() const;
    bool isStringable() const;

    bool isPrimitive() const;

    Var copy() const;
};
