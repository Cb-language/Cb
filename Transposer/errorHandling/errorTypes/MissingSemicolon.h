#pragma once
#include "../Error.h"

class MissingSemicolon : public Error
{
public:
    MissingSemicolon(const Token &token);
};