#pragma once
#include "errorHandling/Error.h"

class UnrecognizedIdentifier : public Error
{
public:
    UnrecognizedIdentifier(const Token& token);
};
