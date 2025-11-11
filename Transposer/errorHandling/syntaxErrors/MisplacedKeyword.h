#pragma once
#include "../Error.h"

class MisplacedKeyword : public Error
{
public:
    MisplacedKeyword(int line, int column, const std::string &token = "");
};