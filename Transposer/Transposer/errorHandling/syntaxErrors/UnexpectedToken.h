#pragma once
#include "../Error.h"

class UnexpectedToken : public Error
{
public:
    UnexpectedToken(const Token &token);
};