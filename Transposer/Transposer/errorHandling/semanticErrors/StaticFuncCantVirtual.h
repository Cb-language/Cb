#pragma once
#include "errorHandling/Error.h"

class StaticFuncCantVirtual : public Error
{
public:
    StaticFuncCantVirtual(const Token& token);
};