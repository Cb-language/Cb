#pragma once
#include <vector>

#include "Type.h"
#include "Var.h"

class Func
{
private:
    const Type rType;
    const std::wstring funcName;
    const std::vector<Var> args;

public:
    Func(const Type& rType, const std::wstring& funcName, const std::vector<Var>& args);

    const std::vector<Var>& getArgs() const;
    const std::wstring& getFuncName() const;
    const Type& getType() const;

    std::string translateToCpp() const;

    bool operator==(const Func& other) const;

    bool isLegalCall(const Func& other) const;
};
