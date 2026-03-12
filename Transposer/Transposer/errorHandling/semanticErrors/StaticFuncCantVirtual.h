#pragma once
#include "errorHandling/Error.h"

class StaticFuncCantVirtual : public Error
{
public:
    explicit StaticFuncCantVirtual(const Token& token);
};