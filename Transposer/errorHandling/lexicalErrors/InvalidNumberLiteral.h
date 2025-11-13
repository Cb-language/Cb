#pragma once
#include "../Error.h"

class InvalidNumberLiteral : public Error
{
public:
    InvalidNumberLiteral(const Token &token);
};