#pragma once
#include "../Error.h"

class InvalidStatement : public Error
{
public:
    InvalidStatement(int line, int column, const std::string &token = "");
};