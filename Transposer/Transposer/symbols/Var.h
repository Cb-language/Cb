#pragma once
#include <memory>
#include <string>
#include "Type/PrimitiveType.h"

class Var
{
private:
    std::unique_ptr<IType> type;
    std::string name;
    bool isStatic;

public:
    Var(std::unique_ptr<IType> type, const std::string& name, bool isStatic = false);
    Var(const Var& other);
    std::unique_ptr<IType> getType() const;
    std::string getName() const;
    bool getStatic() const;
    void setStatic(bool isStatic);

    bool operator==(const Var& other) const;

    bool operator==(const std::string& other) const;
    void operator=(const Var& other);

    bool isNumberable() const;
    bool isStringable() const;

    bool isPrimitive() const;
    void setIsStatic(const bool isStatic);

    Var copy() const;
};
