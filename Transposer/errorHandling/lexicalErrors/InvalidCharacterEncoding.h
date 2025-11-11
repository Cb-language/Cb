#pragma once
#include "../Error.h"

class InvalidCharacterEncoding : public Error
{
public:
    InvalidCharacterEncoding(int line, int column, const std::string& token = "");
};