#pragma once
#include <vector>

#include "Var.h"

class Constractor
{
private:
    const std::wstring className;
    std::vector<Var> args;

public:
    Constractor(const std::vector<Var>& args, const std::wstring& className);

    const std::vector<Var>& getArgs() const;
    const std::wstring& getClassName() const;

    std::string translateToCpp() const;

    bool operator==(const Constractor& other) const;

    bool isLegalCall(const Constractor& other) const;

    Constractor copy() const;
};
