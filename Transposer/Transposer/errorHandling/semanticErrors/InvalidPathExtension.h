#pragma once
#include "errorHandling/Error.h"

class InvalidPathExtension : public Error
{
public:
    explicit InvalidPathExtension(const Token& token);
};
