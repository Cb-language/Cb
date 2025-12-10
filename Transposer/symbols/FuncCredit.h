#pragma once
#include <string>

#include "Func.h"
#include "token/Token.h"

class FuncCredit
{
private:
    const std::wstring name;
    const Token token;

public:
    FuncCredit(const std::wstring& name, const Token& token);
    FuncCredit(const FuncCredit& funcCredit);

    const Token& getToken() const;

    bool isLegalCredit(const Func& func) const;
};
