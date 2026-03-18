#pragma once
#include "errorHandling/Error.h"

class InstantiateAbstractClass : public Error
{
public:
    explicit InstantiateAbstractClass(const Token& token);
};
