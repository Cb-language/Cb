#pragma once
#include "errorHandling/Error.h"

class InvalidNumberLiteral : public Error
{
public:
    InvalidNumberLiteral(const Token& token);
};
