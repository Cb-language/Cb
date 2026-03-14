#pragma once
#include "errorHandling/Error.h"

class ElseWithoutIf : public Error
{
public:
    explicit ElseWithoutIf(const Token &token);
};
