#pragma once
#include "errorHandling/Error.h"

class NoRest : public Error
{
public:
    explicit NoRest(const Token& token);
};
