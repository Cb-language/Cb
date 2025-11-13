#pragma once
#include "../Error.h"

class InvalidEscapeSequence : public Error
{
public:
    InvalidEscapeSequence(const Token &token);
};