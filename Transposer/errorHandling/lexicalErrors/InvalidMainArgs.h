#pragma once
#include "errorHandling/Error.h"

class InvalidMainArgs : Error
{
public:
    InvalidMainArgs(const Token &token);;
};
