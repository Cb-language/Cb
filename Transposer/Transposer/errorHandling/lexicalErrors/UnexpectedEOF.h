#pragma once
#include "../Error.h"

class UnexpectedEOF : public Error
{
public:
    explicit UnexpectedEOF(const Token &token);
};