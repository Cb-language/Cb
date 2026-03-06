#pragma once
#include "errorHandling/Error.h"

class InvalidOverrideSignature : public Error
{
public:
    InvalidOverrideSignature(const Token& token);
};
