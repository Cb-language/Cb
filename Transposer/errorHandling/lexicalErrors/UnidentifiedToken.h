#pragma once
#include "../Error.h"

class UnidentifiedToken : public Error
{
public:
    UnidentifiedToken(const Token &token);
};

