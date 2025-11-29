#pragma once
#include "../Error.h"

class InvalidStatement : public Error
{
public:
    InvalidStatement(const Token &token);
};