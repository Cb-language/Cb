#pragma once
#include "errorHandling/Error.h"

class NoOverrideError : public Error
{
public:
    explicit NoOverrideError(const Token& token);
};
