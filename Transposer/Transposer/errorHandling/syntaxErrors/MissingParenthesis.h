#pragma once
#include "../Error.h"

class MissingParenthesis : public Error
{
public:
    explicit MissingParenthesis(const Token &token);
};