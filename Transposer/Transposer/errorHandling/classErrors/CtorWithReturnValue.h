#pragma once
#include "errorHandling/Error.h"

class CtorWithReturnValue : public Error
{
public:
    explicit CtorWithReturnValue(const Token& token);
};
