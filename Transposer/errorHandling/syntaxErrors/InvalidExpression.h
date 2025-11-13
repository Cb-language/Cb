#pragma once
#include "../Error.h"

class InvalidExpression : public Error
{
public:
    InvalidExpression(const Token &token);
};