#pragma once
#include "errorHandling/Error.h"

class InvalidMainReturnType : Error
{
public:
    InvalidMainReturnType(const Token& token);
};
