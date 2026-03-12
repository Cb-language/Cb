#pragma once
#include "errorHandling/Error.h"

class InvalidMainArgs : public Error
{
public:
    explicit InvalidMainArgs(const Token &token);
};
