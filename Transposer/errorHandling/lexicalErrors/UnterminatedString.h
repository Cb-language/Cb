#pragma once
#include "../Error.h"

class UnterminatedString : public Error
{
public:
    UnterminatedString(int line, int column, const std::string &token = "");
};