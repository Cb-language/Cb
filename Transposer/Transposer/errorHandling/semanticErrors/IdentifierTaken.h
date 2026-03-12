#pragma once
#include "../Error.h"
class IdentifierTaken : public Error
{
public:
    explicit IdentifierTaken(const Token &token);
};