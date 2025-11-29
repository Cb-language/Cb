#pragma once
#include "../Error.h"

class InvalidCharacterEncoding : public Error
{
public:
    InvalidCharacterEncoding(const Token &token);
};