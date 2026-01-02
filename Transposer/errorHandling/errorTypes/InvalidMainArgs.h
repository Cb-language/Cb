#pragma once
#include "errorHandling/Error.h"

class InvalidMainArgs : public Error
{
public:
    InvalidMainArgs(const Token &token);
};
