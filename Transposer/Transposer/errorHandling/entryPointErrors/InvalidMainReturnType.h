#pragma once
#include "errorHandling/Error.h"

class InvalidMainReturnType : public Error
{
public:
    explicit InvalidMainReturnType(const Token& token);
};
