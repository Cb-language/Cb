#pragma once
#include "errorHandling/Error.h"

class NoPlacementOperator : public Error
{
public:
    explicit NoPlacementOperator(const Token& token);
};
