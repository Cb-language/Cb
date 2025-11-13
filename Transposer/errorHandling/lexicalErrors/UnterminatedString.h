#pragma once
#include "../Error.h"

class UnterminatedString : public Error
{
public:
    UnterminatedString(const Token &token);
};