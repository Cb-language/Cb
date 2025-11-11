#pragma once
#include "../Error.h"

class InvalidEscapeSequence : public Error
{
public:
    InvalidEscapeSequence(int line, int column, const std::string &token = "");
};