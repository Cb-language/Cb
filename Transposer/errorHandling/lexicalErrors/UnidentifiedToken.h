#pragma once
#include "../Error.h"

class UnidentifiedToken : public Error
{
public:
    UnidentifiedToken(int line, int column, const std::string &token = "");
};

