#pragma once
#include <vector>

#include "Type/Type.h"
#include "Var.h"

class Func
{
private:
    std::unique_ptr<IType> rType;
    const std::wstring funcName;
    std::vector<Var> args;

public:
    Func(std::unique_ptr<IType> rType, const std::wstring& funcName, const std::vector<Var>& args);

    const std::vector<Var>& getArgs() const;
    const std::wstring& getFuncName() const;
    std::unique_ptr<IType> getType() const;

    std::string translateToCpp() const;

    bool operator==(const Func& other) const;

    bool isLegalCall(const Func& other) const;

    Func copy() const;
};
