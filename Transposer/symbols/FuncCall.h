#pragma once
#include <string>
#include <vector>

#include "Func.h"
#include "token/Token.h"

class FuncCall
{
private:
    Type type = Type(L"fermata");
    const std::wstring name;
    const std::vector<std::wstring> args;
    const Token token;

public:
    FuncCall(const std::wstring& name, const std::vector<std::wstring>& args, const Token& token);
    FuncCall(const std::wstring& name, const Token& token);
    FuncCall(const FuncCall& other);

    bool isLegalCall(const Func& other) const;

    const Token& getToken() const;
    
    const Type& getType() const;
    void setType(const Type& type);
    
    std::string translateToCpp() const;
};
