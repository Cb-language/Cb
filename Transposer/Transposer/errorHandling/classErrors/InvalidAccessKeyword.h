#pragma once
#include "errorHandling/Error.h"

class InvalidAccessKeyword : public Error
{
public:
    explicit InvalidAccessKeyword(const Token& token);
};
