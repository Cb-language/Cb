#pragma once
#include "errorHandling/Error.h"

class NoRest : public Error
{
public:
    NoRest(const Token& token);
};
