#pragma once
#include "errorHandling/Error.h"

class UnrecognizedIdentifier : public Error
{
public:
    explicit UnrecognizedIdentifier(const Token& token);
};
