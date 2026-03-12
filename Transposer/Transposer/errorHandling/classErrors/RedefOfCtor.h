#pragma once
#include "errorHandling/Error.h"

class RedefOfCtor : public Error
{
public:
    explicit RedefOfCtor(const Token& token);
};
