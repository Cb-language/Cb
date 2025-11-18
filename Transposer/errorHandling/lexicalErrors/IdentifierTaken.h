#pragma once
#include "../Error.h"
class IdentifierTaken : Error
{
public:
    IdentifierTaken(const Token &token);
};