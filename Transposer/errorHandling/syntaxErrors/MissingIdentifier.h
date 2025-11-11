#pragma once
#include "../Error.h"

class MissingIdentifier : public Error
{
public:
    MissingIdentifier(int line, int column, const std::string &token = "");
};