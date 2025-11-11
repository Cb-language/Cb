#pragma once

#include "ErrorHandler.h"
#include <utility>

class Error
{
protected:
    int line = 0;
    int column = 0;
    std::string errorMessage;
    std::string token;

public:
    Error(int line, int column, std::string errorMessage, std::string token = "");
    virtual ~Error() = default;
    void print() const;
};
