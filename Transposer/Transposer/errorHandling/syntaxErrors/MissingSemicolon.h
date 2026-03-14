#pragma once
#include "../Error.h"

class MissingSemicolon : public Error
{
public:
    explicit MissingSemicolon(const Token &token);
};