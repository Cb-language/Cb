#pragma once

#include "ErrorHandler.h"
#include <utility>

class Error : public std::exception
{
protected:
    int line = 0;
    int column = 0;
    std::string errorMessage;
    std::string token;
    std::string fullMessage;
public:
    Error(int line, int column, std::string errorMessage, std::string token = "");
    virtual ~Error() = default;
    void print() const;
    const char* what() const noexcept override;
};