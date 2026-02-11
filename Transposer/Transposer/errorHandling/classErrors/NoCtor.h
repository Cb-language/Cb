#pragma once
#include "errorHandling/Error.h"

class NoCtor : public Error
{
public:
    NoCtor(const Token& token);
};
