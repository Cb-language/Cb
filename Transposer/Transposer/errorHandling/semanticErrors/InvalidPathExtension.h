#pragma once
#include "errorHandling/Error.h"

class InvalidPathExtension : public Error
{
public:
    InvalidPathExtension(const Token& token);
};
