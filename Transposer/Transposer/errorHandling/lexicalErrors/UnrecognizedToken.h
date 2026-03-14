#pragma once
#include "errorHandling/Error.h"

class UnrecognizedToken : public Error
{
public:
    explicit UnrecognizedToken(const Token& token);
};
