#pragma once
#include "../Error.h"

class NoReturn : public Error
{
public:
    explicit NoReturn(const Token& token);
};
