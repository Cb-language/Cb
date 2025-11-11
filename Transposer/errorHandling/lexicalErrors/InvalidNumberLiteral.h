#pragma once
#include "../Error.h"

class InvalidNumberLiteral : public Error
{
public:
    InvalidNumberLiteral(int line, int column, const std::string &token = "");
};