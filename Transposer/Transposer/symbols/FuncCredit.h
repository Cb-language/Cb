#pragma once
#include <string>

#include "Func.h"
#include "token/Token.h"

class FuncCredit
{
private:
    const FQN name;
    const Token token;

public:
    FuncCredit(const FQN& name, const Token& token);
    FuncCredit(const FuncCredit& funcCredit);

    const Token& getToken() const;
    const FQN& getName() const;

    bool isLegalCredit(const Func& func) const;
};
