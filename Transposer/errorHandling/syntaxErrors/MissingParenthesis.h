#pragma once
#include "../Error.h"

class MissingParenthesis : public Error
{
public:
    MissingParenthesis(int line, int column, const std::string &token = "");
};