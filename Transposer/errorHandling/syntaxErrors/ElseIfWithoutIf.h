#pragma once
#include "errorHandling/Error.h"

class ElseIfWithoutIf : public Error
{
public:
    ElseIfWithoutIf(const Token &token);
};
