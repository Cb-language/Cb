#pragma once
#include "errorHandling/Error.h"

class FuncInsideFunc : public Error
{
public:
    explicit FuncInsideFunc(const Token& token);
};
