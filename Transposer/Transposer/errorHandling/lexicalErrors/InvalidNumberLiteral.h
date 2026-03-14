#pragma once
#include "errorHandling/Error.h"

class InvalidNumberLiteral : public Error
{
public:
    explicit InvalidNumberLiteral(const Token& token);
};
