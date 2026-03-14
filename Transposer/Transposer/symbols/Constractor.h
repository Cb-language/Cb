#pragma once
#include <vector>

#include "Var.h"

class Constractor
{
private:
    FQN className;
    std::vector<Var> args;

public:
    Constractor(const std::vector<Var>& args, const FQN& className);

    const std::vector<Var>& getArgs() const;
    const FQN& getClassName() const;

    std::string translateToCpp() const;

    bool operator==(const Constractor& other) const;

    bool isLegalCall(const Constractor& other) const;

    Constractor copy() const;
};
