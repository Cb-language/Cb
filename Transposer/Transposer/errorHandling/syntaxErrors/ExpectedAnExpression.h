#pragma once
#include "errorHandling/Error.h"

class ExpectedAnExpression : public Error
{
public:
    explicit ExpectedAnExpression(const Token& token);
};
