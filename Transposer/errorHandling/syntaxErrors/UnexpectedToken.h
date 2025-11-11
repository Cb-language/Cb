#pragma once
#include "../Error.h"

class UnexpectedToken : public Error
{
public:
    UnexpectedToken(int line, int column, const std::string &token = "");
};