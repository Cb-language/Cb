#pragma once
#include "../Error.h"

class UnexpectedToken : public Error
{
public:
    explicit UnexpectedToken(const Token &token);
};