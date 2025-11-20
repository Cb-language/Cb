#pragma once
#include "../Error.h"
class IdentifierTaken : public Error
{
public:
    IdentifierTaken(const Token &token);
};