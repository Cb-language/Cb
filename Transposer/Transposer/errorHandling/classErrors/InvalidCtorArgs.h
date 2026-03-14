#pragma once
#include "errorHandling/Error.h"

class InvalidCtorArgs : public Error
{
public:
    explicit InvalidCtorArgs(const Token& token);
};
