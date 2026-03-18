#pragma once
#include "errorHandling/Error.h"

class InvalidOverrideSignature : public Error
{
public:
    explicit InvalidOverrideSignature(const Token& token);
};
