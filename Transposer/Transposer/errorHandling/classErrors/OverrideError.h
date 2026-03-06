#pragma once
#include "errorHandling/Error.h"

class OverrideError : public Error
{
public:
    explicit OverrideError(const Token& token);
};
