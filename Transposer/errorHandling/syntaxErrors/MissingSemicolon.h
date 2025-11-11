#pragma once
#include "../Error.h"

class MissingSemicolon : public Error
{
public:
    MissingSemicolon(int line, int column, const std::string &token = "");
};