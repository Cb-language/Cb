#pragma once
#include "errorHandling/Error.h"

class InvalidAccessKeyword : public Error
{
public:
    InvalidAccessKeyword(const Token& token);
};
