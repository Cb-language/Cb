#pragma once
#include "errorHandling/Error.h"

class IllegalVarName : public Error
{
public:
    explicit IllegalVarName(const Token& token);
};
