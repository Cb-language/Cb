#pragma once

#include "ErrorHandler.h"
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
    Error(const Token &token, std::string errorMessage);
    virtual ~Error() = default;
    void print() const;
    const char* what() const noexcept override;
};