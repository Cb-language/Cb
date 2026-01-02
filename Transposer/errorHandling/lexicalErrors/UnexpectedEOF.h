#pragma once
#include "../Error.h"

class UnexpectedEOF : public Error
{
public:
    UnexpectedEOF(const Token &token);
};