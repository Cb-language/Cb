#pragma once
#include "errorHandling/Error.h"

class ExpectedAPath : public Error
{
public:
    explicit ExpectedAPath(const Token& token);
};
