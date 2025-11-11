#pragma once
#include "../Error.h"

class InvalidExpression : public Error
{
public:
    InvalidExpression(int line, int column, const std::string &token = "");
};