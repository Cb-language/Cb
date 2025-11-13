#pragma once
#include "../Error.h"

class MissingBrace : public Error
{
public:
    MissingBrace(const Token &token);
};