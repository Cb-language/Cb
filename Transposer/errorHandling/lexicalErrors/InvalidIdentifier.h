#pragma once
#include "errorHandling/Error.h"
#include "token/Token.h"

class InvalidIdentifier : public Error
{
public:
    InvalidIdentifier(const Token &token);
};
