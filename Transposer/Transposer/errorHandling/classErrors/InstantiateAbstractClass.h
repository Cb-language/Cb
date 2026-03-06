#pragma once
#include "errorHandling/Error.h"

class InstantiateAbstractClass : public Error
{
public:
    InstantiateAbstractClass(const Token& token);
};
