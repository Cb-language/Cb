#pragma once
#include <string>
#include <vector>

#include "Func.h"
#include "Var.h"
#include "token/Token.h"

class FuncCall
{
private:
    const std::wstring name;
    const std::vector<Var> args;
    const Token token;

public:
    FuncCall(const std::wstring& name, const std::vector<Var>& args, const Token& token);
    FuncCall(const std::wstring& name, const Token& token);
    FuncCall(const FuncCall& other);

    bool isLegalCall(const Func& other) const;

    const Token& getToken() const;
};
