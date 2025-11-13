#pragma once
#include "../Error.h"

class MissingIdentifier : public Error
{
public:
    MissingIdentifier(const Token &token);
};