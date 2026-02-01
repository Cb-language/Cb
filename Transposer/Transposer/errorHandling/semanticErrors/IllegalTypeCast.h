#pragma once
#include "../Error.h"

struct Token;

class IllegalTypeCast : public Error
{
public:
    IllegalTypeCast(const Token& token, const std::string& typeFrom, const std::string& typeTo);
};
