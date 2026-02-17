#pragma once
#include "errorHandling/Error.h"

class RedefOfCtor : public Error
{
public:
    RedefOfCtor(const Token& token);
};
