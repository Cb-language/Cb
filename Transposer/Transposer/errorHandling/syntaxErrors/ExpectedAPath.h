#pragma once
#include "errorHandling/Error.h"

class ExpectedAPath : public Error
{
public:
    ExpectedAPath(const Token& token);
};
