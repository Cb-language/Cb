#pragma once
#include "errorHandling/Error.h"

class NoCtor : public Error
{
public:
    explicit NoCtor(const Token& token);
};
