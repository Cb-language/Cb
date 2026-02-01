#pragma once
#include "errorHandling/Error.h"

class ExpectedAnExpression : public Error
{
public:
    ExpectedAnExpression(const Token& token);
};
