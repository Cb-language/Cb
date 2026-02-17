#pragma once
#include "errorHandling/Error.h"

class InvalidCtorArgs : public Error
{
public:
    InvalidCtorArgs(const Token& token);
};
