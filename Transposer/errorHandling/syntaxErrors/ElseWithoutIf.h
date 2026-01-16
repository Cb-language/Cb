#pragma once
#include "errorHandling/Error.h"

class ElseWithoutIf : public Error
{
public:
    ElseWithoutIf(const Token &token);
};
