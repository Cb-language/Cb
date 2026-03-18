#pragma once
#include "../Error.h"

class WrongReturnType : public Error
{
public:
    explicit WrongReturnType(const Token &token);
};