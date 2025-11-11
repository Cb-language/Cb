#pragma once
#include "../Error.h"

class UnexpectedEOF : public Error
{
public:
    UnexpectedEOF(int line, int column, const std::string &token = "");
};