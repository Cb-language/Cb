#pragma once
#include "errorHandling/Error.h"

class OverrideError : public Error
{
public:
    OverrideError(const Token& token);
};
