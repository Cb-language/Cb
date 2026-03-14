#pragma once
#include "../Error.h"

class MissingIdentifier : public Error
{
public:
    explicit MissingIdentifier(const Token &token);
};