#pragma once
#include "../Error.h"

class MissingBrace : public Error
{
public:
    explicit MissingBrace(const Token &token);
};