#pragma once
#include "../Error.h"

class MissingBrace : public Error
{
public:
    MissingBrace(int line, int column, const std::string &token = "");
};