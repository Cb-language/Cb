#pragma once
#include <string>

#include "Func.h"
#include "token/Token.h"

class FuncCredit
{
private:
    const std::string name;
    const Token token;

public:
    FuncCredit(const std::string& name, const Token& token);
    FuncCredit(const FuncCredit& funcCredit);

    const Token& getToken() const;
    const std::string& getName() const;

    bool isLegalCredit(const Func& func) const;
};
