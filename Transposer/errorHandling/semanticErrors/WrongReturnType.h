#pragma once
#include "../Error.h"

class WrongReturnType : public Error
{
public:
    WrongReturnType(const Token &token);
};