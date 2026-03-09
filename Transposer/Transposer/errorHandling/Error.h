#pragma once

#include "token/Token.h"
#include <iostream>
#include <locale>
#include <codecvt>
#include <unordered_map>

class Error : public std::exception
{
private:
    static std::unordered_map<TokenType, std::string> tokenToTypeMap;
    static std::unordered_map<TokenType, std::string> typeToStr();
protected:
    Token token;
    std::string errorMessage;
    std::string fullMessage;

public:
    Error(const Token &token, const std::string& errorMessage);
    ~Error() override = default;
    void print() const;
    const char* what() const noexcept override;
    const Token& getToken() const;
    const std::string& getErrorMessage() const;
    Error* copy() const;
};
