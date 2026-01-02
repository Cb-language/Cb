#pragma once
#include "errorHandling/Error.h"

class UnrecognizedToken : public Error
{
public:
    UnrecognizedToken(const Token& token);
};
