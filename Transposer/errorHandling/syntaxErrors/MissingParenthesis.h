#pragma once
#include "../Error.h"

class MissingParenthesis : public Error
{
public:
    MissingParenthesis(const Token &token);
};