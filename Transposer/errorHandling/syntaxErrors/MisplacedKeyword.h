#pragma once
#include "../Error.h"

class MisplacedKeyword : public Error
{
public:
    MisplacedKeyword(const Token &token);
};