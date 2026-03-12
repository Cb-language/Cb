#pragma once
#include "../Error.h"

class InvalidExpression : public Error
{
public:
    explicit InvalidExpression(const Token &token);
};