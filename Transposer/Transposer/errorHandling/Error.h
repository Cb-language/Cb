#pragma once

#include "token/Token.h"
#include <iostream>
#include <locale>
#include <codecvt>

class Error : public std::exception
{
protected:
    Token token;
    std::string errorMessage;
    std::string fullMessage;
public:
    Error(const Token &token, const std::string& errorMessage);
    ~Error() override = default;
    void print() const;
    const char* what() const noexcept override;
};