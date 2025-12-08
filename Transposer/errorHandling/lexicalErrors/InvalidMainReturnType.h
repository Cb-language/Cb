#pragma once
#include "errorHandling/Error.h"

class InvalidMainReturnType : public Error
{
public:
    InvalidMainReturnType(const Token& token);
};
