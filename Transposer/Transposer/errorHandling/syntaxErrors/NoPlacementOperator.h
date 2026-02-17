#pragma once
#include "errorHandling/Error.h"

class NoPlacementOperator : public Error
{
public:
    NoPlacementOperator(const Token& token);
};
